#include "storage/kv_store.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "core/log_tags.h"

esp_err_t kv_set_i32(const char *ns, const char *key, int32_t v){
    nvs_handle_t h; esp_err_t err = nvs_open(ns, NVS_READWRITE, &h);
    if (err != ESP_OK) return err;
    err = nvs_set_i32(h, key, v);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return err;
}
esp_err_t kv_get_i32(const char *ns, const char *key, int32_t *out){
    if (!out) return ESP_ERR_INVALID_ARG;
    nvs_handle_t h; esp_err_t err = nvs_open(ns, NVS_READONLY, &h);
    if (err != ESP_OK) return err;
    err = nvs_get_i32(h, key, out);
    nvs_close(h);
    return err;
}

esp_err_t kv_set_u8(const char *ns, const char *key, uint8_t v){
    nvs_handle_t h; esp_err_t err = nvs_open(ns, NVS_READWRITE, &h);
    if (err != ESP_OK) return err;
    err = nvs_set_u8(h, key, v);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return err;
}
esp_err_t kv_get_u8(const char *ns, const char *key, uint8_t *out){
    if (!out) return ESP_ERR_INVALID_ARG;
    nvs_handle_t h; esp_err_t err = nvs_open(ns, NVS_READONLY, &h);
    if (err != ESP_OK) return err;
    err = nvs_get_u8(h, key, out);
    nvs_close(h);
    return err;
}

esp_err_t kv_set_blob(const char *ns, const char *key, const void *buf, size_t len){
    nvs_handle_t h; esp_err_t err = nvs_open(ns, NVS_READWRITE, &h);
    if (err != ESP_OK) return err;
    err = nvs_set_blob(h, key, buf, len);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return err;
}
esp_err_t kv_get_blob(const char *ns, const char *key, void *buf, size_t *inout_len){
    if (!inout_len) return ESP_ERR_INVALID_ARG;
    nvs_handle_t h; esp_err_t err = nvs_open(ns, NVS_READONLY, &h);
    if (err != ESP_OK) return err;
    err = nvs_get_blob(h, key, buf, inout_len);
    nvs_close(h);
    return err;
}
