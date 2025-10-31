#include "tasks/task_time_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/log_tags.h"
#include "esp_log.h"
#include <time.h>

static void task(void *arg) {
    (void)arg;
    while (1) {
        time_t now; struct tm t;
        time(&now); localtime_r(&now, &t);
        ESP_LOGI(TAG_TIME, "Hora: %02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void start_task_time_log(void) {
    xTaskCreate(task, "task_time_log", 4096, NULL, 2, NULL);
}
