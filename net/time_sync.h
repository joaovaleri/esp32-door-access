#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Inicia SNTP com pool.ntp.org
void time_sync_start(void);

// Bloqueia até o relógio estar válido (ou timeout). Retorna ESP_OK se ok.
esp_err_t time_sync_wait_synced(int max_retries, int delay_ms);

// Aplica timezone (usa DEFAULT_TZ do config.h se tz==NULL)
void time_apply_timezone(const char *tz);

#ifdef __cplusplus
}
#endif
