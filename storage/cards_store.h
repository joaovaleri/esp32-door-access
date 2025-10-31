#pragma once
#include "esp_err.h"
#include "core/app_ctx.h"  // uid_t e limites

#ifdef __cplusplus
extern "C" {
#endif

// Carrega todos os cartões e a contagem para o ctx (usa NVS_KEY_CARDS e NVS_KEY_CARD_CNT)
esp_err_t cards_load_all(app_ctx_t *ctx);

// Persiste o array (ctx->authorized_cards) e a contagem (ctx->authorized_count)
esp_err_t cards_save_all(const app_ctx_t *ctx);

// Utilitários de lista em memória (não salvam automaticamente)
bool cards_find(const app_ctx_t *ctx, const uid_t *uid, int *index);
bool cards_add(app_ctx_t *ctx, const uid_t *uid);      // retorna true se inseriu
bool cards_remove(app_ctx_t *ctx, const uid_t *uid);   // retorna true se removeu

#ifdef __cplusplus
}
#endif
