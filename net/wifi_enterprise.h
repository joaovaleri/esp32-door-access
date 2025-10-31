#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Inicializa Wi-Fi STA, configura EAP e bloqueia até obter IP (ou timeout).
esp_err_t wifi_enterprise_start_and_wait(void);

#ifdef __cplusplus
}
#endif
