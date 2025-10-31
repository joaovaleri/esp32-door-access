#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "core/app_ctx.h"
#include "core/config.h"
#include "core/log_tags.h"

#include "net/wifi_enterprise.h"
#include "net/time_sync.h"

#include "drivers/door.h"
#include "drivers/rfid.h"

#include "storage/counter_store.h"
#include "storage/cards_store.h"
#include "storage/master_key.h"

#include "domain/holidays.h"

#include "tasks/task_pulse.h"
#include "tasks/task_reset.h"
#include "tasks/task_agenda_fetch.h"
#include "tasks/task_agenda_exec.h"
#include "tasks/task_time_log.h"

void app_main(void) {
    // NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    app_ctx_t ctx;
    app_ctx_init(&ctx);

    // Rede + hora
    ESP_ERROR_CHECK(wifi_enterprise_start_and_wait());
    time_sync_start();
    ESP_ERROR_CHECK(time_sync_wait_synced(20, 2000));
    time_apply_timezone(NULL); // DEFAULT_TZ em config.h

    // Drivers
    door_init();
    rfid_init(&ctx);

    // Storage
    master_key_init_from_menuconfig();
    ctx.counter = counter_load_or_default();
    ESP_ERROR_CHECK(cards_load_all(&ctx));

    // Feriados
    holidays_download_and_cache(&ctx);

    // Tasks
    start_task_agenda_fetch(&ctx);
    start_task_agenda_exec(&ctx);
    start_task_pulse(&ctx);
    start_task_reset(&ctx);
    start_task_time_log();

    ESP_LOGI(TAG_APP, "Sistema iniciado.");
}
