#include "storage/cards_store.h"
#include "storage/kv_store.h"
#include "core/config.h"
#include <string.h>

esp_err_t cards_load_all(app_ctx_t *ctx) {
    if (!ctx) return ESP_ERR_INVALID_ARG;

    size_t need = sizeof(ctx->authorized_cards);
    memset(ctx->authorized_cards, 0, need);
    ctx->authorized_count = 0;

    // blob de cartões
    esp_err_t err = kv_get_blob(NVS_NS, NVS_KEY_CARDS, ctx->authorized_cards, &need);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // contador
    uint8_t cnt = 0;
    err = kv_get_u8(NVS_NS, NVS_KEY_CARD_CNT, &cnt);
    if (err == ESP_OK) {
        if (cnt > MAX_AUTHORIZED_CARDS) cnt = 0; // saneamento
        ctx->authorized_count = cnt;
    }
    return ESP_OK;
}

esp_err_t cards_save_all(const app_ctx_t *ctx) {
    if (!ctx) return ESP_ERR_INVALID_ARG;
    esp_err_t err = kv_set_blob(NVS_NS, NVS_KEY_CARDS, ctx->authorized_cards, sizeof(ctx->authorized_cards));
    if (err != ESP_OK) return err;
    return kv_set_u8(NVS_NS, NVS_KEY_CARD_CNT, ctx->authorized_count);
}

static bool uid_equal(const uid_t *a, const uid_t *b) {
    if (!a || !b) return false;
    if (a->length != b->length) return false;
    return memcmp(a->value, b->value, a->length) == 0;
}

bool cards_find(const app_ctx_t *ctx, const uid_t *uid, int *index) {
    if (!ctx || !uid) return false;
    for (int i = 0; i < ctx->authorized_count; ++i) {
        if (uid_equal(&ctx->authorized_cards[i], uid)) {
            if (index) *index = i;
            return true;
        }
    }
    return false;
}

bool cards_add(app_ctx_t *ctx, const uid_t *uid) {
    if (!ctx || !uid) return false;
    if (ctx->authorized_count >= MAX_AUTHORIZED_CARDS) return false;
    if (cards_find(ctx, uid, NULL)) return false; // já existe
    ctx->authorized_cards[ctx->authorized_count++] = *uid;
    return true;
}

bool cards_remove(app_ctx_t *ctx, const uid_t *uid) {
    if (!ctx || !uid) return false;
    int idx = -1;
    if (!cards_find(ctx, uid, &idx)) return false;
    for (int j = idx; j < (int)ctx->authorized_count - 1; ++j) {
        ctx->authorized_cards[j] = ctx->authorized_cards[j + 1];
    }
    ctx->authorized_count--;
    return true;
}
