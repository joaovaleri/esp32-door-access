#include "net/wifi_enterprise.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"

#include "core/config.h"
#include "core/log_tags.h"

static EventGroupHandle_t s_wifi_event_group;
static const int WIFI_CONNECTED_BIT = BIT0;

static void wifi_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG_WIFI, "Wi-Fi desconectado. Re-tentando...");
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG_WIFI, "IP obtido.");
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t wifi_enterprise_start_and_wait(void) {
    esp_err_t err;

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = { 0 };
    strncpy((char*)wifi_config.sta.ssid, WIFI_SSID, sizeof(wifi_config.sta.ssid));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_ENTERPRISE;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Configuração EAP (TTLS/PAP, PEAP/MSCHAPv2, etc. – aqui só credenciais básicas)
    ESP_ERROR_CHECK(esp_eap_client_set_identity((const uint8_t*)EAP_IDENTITY, strlen(EAP_IDENTITY)));
    ESP_ERROR_CHECK(esp_eap_client_set_username((const uint8_t*)EAP_USERNAME, strlen(EAP_USERNAME)));
    ESP_ERROR_CHECK(esp_eap_client_set_password((const uint8_t*)EAP_PASSWORD, strlen(EAP_PASSWORD)));
    ESP_ERROR_CHECK(esp_wifi_sta_enterprise_enable());

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WIFI, "Conectando a SSID='%s' via WPA2-Enterprise...", WIFI_SSID);
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(30000));
    if ((bits & WIFI_CONNECTED_BIT) == 0) {
        ESP_LOGE(TAG_WIFI, "Timeout ao obter IP.");
        err = ESP_ERR_TIMEOUT;
    } else {
        err = ESP_OK;
    }
    return err;
}
