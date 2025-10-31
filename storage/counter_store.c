#include "storage/counter_store.h"
#include "storage/kv_store.h"
#include "core/config.h"

int counter_load_or_default(void) {
    int32_t v = CONTADOR_MIN;
    if (kv_get_i32(NVS_NS, NVS_KEY_COUNTER, &v) != ESP_OK) {
        v = CONTADOR_MIN;
    }
    if (v < CONTADOR_MIN || v > CONTADOR_MAX) v = CONTADOR_MIN;
    return (int)v;
}

esp_err_t counter_save(int v) {
    if (v < CONTADOR_MIN || v > CONTADOR_MAX) return ESP_ERR_INVALID_ARG;
    return kv_set_i32(NVS_NS, NVS_KEY_COUNTER, (int32_t)v);
}

int counter_next(int current) {
    int n = current + 1;
    if (n > CONTADOR_MAX) n = CONTADOR_MIN;
    return n;
}
