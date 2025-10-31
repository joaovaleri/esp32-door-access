#include "core/app_ctx.h"

void app_ctx_init(app_ctx_t *ctx) {
    if (!ctx) return;
    memset(ctx, 0, sizeof(*ctx));

    // Defaults
    ctx->counter = CONTADOR_MIN;
    ctx->developer_mode = false;
    ctx->authorized_count = 0;
    ctx->bloqueia_pulso_ate_us = 0;

    // wifi_event_group será criado pelo módulo de Wi-Fi quando necessário
    ctx->wifi_event_group = NULL;

    // Handles de RFID serão criados no driver
    ctx->rfid_driver  = NULL;
    ctx->rfid_scanner = NULL;
}
