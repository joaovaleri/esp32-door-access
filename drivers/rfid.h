#pragma once
#include "core/app_ctx.h"
#include "rc522.h"
#include "rc522_types.h"

// Inicializa o leitor RC522 via SPI e registra eventos
void rfid_init(app_ctx_t *ctx);

// Callback de evento RC522 (quando cartão é detectado)
void rfid_on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *data);
