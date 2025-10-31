#include "tasks/task_agenda_fetch.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/config.h"
#include "core/log_tags.h"
#include "net/http_simple.h"
#include "domain/agenda.h"
#include "esp_log.h"
#include <string.h>

static void task(void *arg) {
    app_ctx_t *ctx = (app_ctx_t*)arg;

    while (1) {
        char *resp = NULL; size_t len = 0;
        if (http_get_alloc(AGENDA_URL, &resp, &len) == ESP_OK && resp) {
            agenda_list_t tmp;
            if (agenda_parse_json_min(resp, &tmp)) {
                ctx->agenda = tmp; // cópia inteira
                ESP_LOGI(TAG_AGENDA, "Agenda atualizada: %d itens", ctx->agenda.count);
            } else {
                ESP_LOGW(TAG_AGENDA, "Falha ao parsear agenda");
            }
        }
        free(resp);
        ESP_LOGI(TAG_AGENDA, "Próximo download em 30 min.");
        vTaskDelay(pdMS_TO_TICKS(30 * 60 * 1000));
    }
}

void start_task_agenda_fetch(app_ctx_t *ctx) {
    xTaskCreate(task, "task_agenda_fetch", 4096, ctx, 4, NULL);
}
