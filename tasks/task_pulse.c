#include "tasks/task_pulse.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/config.h"
#include "core/log_tags.h"
#include "net/http_simple.h"
#include "drivers/door.h"
#include "storage/counter_store.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

// envia o contador atual para COUNTER_URL como texto
static void _push_counter_cloud(int value) {
    char body[12];
    int n = snprintf(body, sizeof(body), "%d", value);
    if (n > 0) (void)http_put_string(COUNTER_URL, body, (size_t)n);
}

static void task(void *arg) {
    app_ctx_t *ctx = (app_ctx_t*)arg;

    while (1) {
        char *resp = NULL; size_t len = 0;
        if (http_get_alloc(TARGET_URL, &resp, &len) == ESP_OK && resp) {
            int alvo = atoi(resp);
            free(resp);

            if (alvo >= CONTADOR_MIN && alvo <= CONTADOR_MAX) {
                int atual = ctx->counter;
                if (alvo != atual) {
                    int pulsos = (alvo - atual + (CONTADOR_MAX - CONTADOR_MIN + 1)) % (CONTADOR_MAX - CONTADOR_MIN + 1);
                    if (pulsos == 0) pulsos = CONTADOR_MAX - CONTADOR_MIN + 1;

                    ESP_LOGI(TAG_TASK_PULSE, "TARGET=%d, atual=%d, pulsos=%d", alvo, atual, pulsos);
                    for (int p = 0; p < pulsos; ++p) {
                        door_pulse();
                        ctx->counter = counter_next(ctx->counter);
                        vTaskDelay(pdMS_TO_TICKS(200));
                    }
                    (void)counter_save(ctx->counter);
                    _push_counter_cloud(ctx->counter);

                    // zera alvo no servidor
                    (void)http_put_string(TARGET_URL, "0", 1);
                } else {
                    ESP_LOGI(TAG_TASK_PULSE, "Estado já era %d; nada a fazer.", atual);
                    (void)http_put_string(TARGET_URL, "0", 1);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void start_task_pulse(app_ctx_t *ctx) {
    xTaskCreate(task, "task_pulse", 4096, ctx, 5, NULL);
}
