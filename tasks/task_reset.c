#include "tasks/task_reset.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/config.h"
#include "core/log_tags.h"
#include "net/http_simple.h"
#include "esp_log.h"
#include "esp_system.h"
#include <stdlib.h>
#include <string.h>

static void task(void *arg) {
    (void)arg;

    while (1) {
        char *resp = NULL; size_t len = 0;
        if (http_get_alloc(RESET_URL, &resp, &len) == ESP_OK && resp) {
            // aceita "1" ou "\"1\"" (Firebase costuma responder com aspas)
            char *val = resp;
            if (val[0] == '\"') val++;
            size_t L = strlen(val);
            if (L && val[L-1] == '\"') val[L-1] = 0;

            int flag = atoi(val);
            ESP_LOGI(TAG_TASK_RESET, "RESET_URL=%d", flag);
            if (flag == 1) {
                (void)http_put_string(RESET_URL, "0", 1);
                vTaskDelay(pdMS_TO_TICKS(200));
                ESP_LOGW(TAG_TASK_RESET, "Reset remoto solicitado. Reiniciando...");
                esp_restart();
            }
        }
        free(resp);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void start_task_reset(app_ctx_t *ctx) {
    xTaskCreate(task, "task_reset", 4096, ctx, 6, NULL);
}
