#include "drivers/rfid.h"
#include "drivers/door.h"
#include "storage/cards_store.h"
#include "storage/counter_store.h"
#include "storage/master_key.h"
#include "net/http_simple.h"
#include "core/log_tags.h"
#include "core/config.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

static void _uid_to_local(const rc522_picc_uid_t *src, uid_t *dst) {
    memset(dst, 0, sizeof(*dst));
    if (!src) return;
    int n = src->length > (int)sizeof(dst->value) ? (int)sizeof(dst->value) : src->length;
    dst->length = n;
    memcpy(dst->value, src->value, n);
}

static void _push_counter_cloud(int value) {
    char body[12];
    int n = snprintf(body, sizeof(body), "%d", value);
    if (n > 0) (void)http_put_string(COUNTER_URL, body, (size_t)n);
}

void rfid_init(app_ctx_t *ctx) {
    ESP_LOGI(TAG_RFID, "Inicializando RC522 SPI...");

    static rc522_spi_config_t driver_config = {
        .host_id = SPI3_HOST,
        .bus_config = &(spi_bus_config_t){
            .miso_io_num = 19,
            .mosi_io_num = 23,
            .sclk_io_num = 18,
        },
        .dev_config = {.spics_io_num = 5},
        .rst_io_num = 22,
    };

    rc522_spi_create(&driver_config, (rc522_driver_handle_t *)&ctx->rfid_driver);
    rc522_driver_install((rc522_driver_handle_t)ctx->rfid_driver);

    rc522_config_t scanner_config = {
        .driver = ctx->rfid_driver,
        .poll_interval_ms = 100,
        .task_stack_size = 4096,
        .task_priority = 5,
    };
    rc522_create(&scanner_config, (rc522_handle_t *)&ctx->rfid_scanner);
    rc522_register_events(ctx->rfid_scanner, RC522_EVENT_PICC_STATE_CHANGED,
                          rfid_on_picc_state_changed, ctx);
    rc522_start(ctx->rfid_scanner);

    ESP_LOGI(TAG_RFID, "RC522 pronto.");
}

void rfid_on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data) {
    app_ctx_t *ctx = (app_ctx_t *)arg;
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)data;
    rc522_picc_t *picc = event->picc;

    if (picc->state != RC522_PICC_STATE_ACTIVE) return;

    char uid_str[RC522_PICC_UID_STR_BUFFER_SIZE_MAX] = {0};
    rc522_picc_uid_to_str(&picc->uid, uid_str, sizeof(uid_str));
    ESP_LOGI(TAG_RFID, "Cartão lido: %s", uid_str);

    // Master key → toggla modo dev
    uid_t local_uid; _uid_to_local(&picc->uid, &local_uid);
    if (master_key_is_uid(&local_uid)) {
        ctx->developer_mode = !ctx->developer_mode;
        ESP_LOGW(TAG_RFID, "Modo desenvolvedor %s", ctx->developer_mode ? "ATIVADO" : "DESATIVADO");
        return;
    }

    if (ctx->developer_mode) {
        // Dev: toggle autorização
        if (!cards_find(ctx, &local_uid, NULL)) {
            if (cards_add(ctx, &local_uid)) {
                (void)cards_save_all(ctx);
                ESP_LOGI(TAG_RFID, "Cartão adicionado (%s).", uid_str);
            } else {
                ESP_LOGW(TAG_RFID, "Lista cheia ao adicionar.");
            }
        } else {
            if (cards_remove(ctx, &local_uid)) {
                (void)cards_save_all(ctx);
                ESP_LOGI(TAG_RFID, "Cartão removido (%s).", uid_str);
            }
        }
        return;
    }

    // Operação normal
    if (!cards_find(ctx, &local_uid, NULL)) {
        ESP_LOGW(TAG_RFID, "Cartão NÃO AUTORIZADO.");
        door_led_red_blink();
        return;
    }

    // Política: alterna 1 <-> 9 (como no seu código)
    int atual = ctx->counter;
    int alvo = (atual == 1) ? 9 : 1;

    if (alvo == atual) {
        ESP_LOGI(TAG_RFID, "Contador já em %d; nada a fazer.", atual);
        return;
    }

    int pulsos = (alvo - atual + (CONTADOR_MAX - CONTADOR_MIN + 1)) % (CONTADOR_MAX - CONTADOR_MIN + 1);
    if (pulsos == 0) pulsos = CONTADOR_MAX - CONTADOR_MIN + 1;

    door_led_green_blink();
    ESP_LOGI(TAG_RFID, "RFID: %d pulsos (%d -> %d)", pulsos, atual, alvo);
    for (int p = 0; p < pulsos; ++p) {
        door_pulse();
        ctx->counter = counter_next(ctx->counter);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    (void)counter_save(alvo);
    _push_counter_cloud(alvo);
    ctx->counter = alvo;
    ESP_LOGI(TAG_RFID, "Contador atualizado: %d", alvo);
}
