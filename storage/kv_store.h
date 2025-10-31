#pragma once
#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// i32
esp_err_t kv_set_i32(const char *ns, const char *key, int32_t v);
esp_err_t kv_get_i32(const char *ns, const char *key, int32_t *out);

// u8
esp_err_t kv_set_u8(const char *ns, const char *key, uint8_t v);
esp_err_t kv_get_u8(const char *ns, const char *key, uint8_t *out);

// blob
esp_err_t kv_set_blob(const char *ns, const char *key, const void *buf, size_t len);
esp_err_t kv_get_blob(const char *ns, const char *key, void *buf, size_t *inout_len);

#ifdef __cplusplus
}
#endif
