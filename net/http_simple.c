#include "net/http_simple.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

#include "core/log_tags.h"

typedef struct {
    char  *buf;
    size_t len;
} http_buf_t;

static esp_err_t _evt_accumulate(esp_http_client_event_t *evt) {
    http_buf_t *hb = (http_buf_t*)evt->user_data;
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt->data_len > 0) {
                char *nbuf = realloc(hb->buf, hb->len + evt->data_len + 1);
                if (!nbuf) return ESP_ERR_NO_MEM;
                hb->buf = nbuf;
                memcpy(hb->buf + hb->len, evt->data, evt->data_len);
                hb->len += evt->data_len;
                hb->buf[hb->len] = 0;
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

esp_err_t http_get_alloc(const char *url, char **out, size_t *out_len) {
    if (!url || !out) return ESP_ERR_INVALID_ARG;
    *out = NULL; if (out_len) *out_len = 0;

    http_buf_t hb = {0};
    esp_http_client_config_t cfg = {
        .url = url,
        .event_handler = _evt_accumulate,
        .user_data = &hb,
        .timeout_ms = 8000,
        .keep_alive_enable = true,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    if (!c) return ESP_FAIL;

    esp_err_t err = esp_http_client_perform(c);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(c);
        if (status < 200 || status >= 300) {
            ESP_LOGE(TAG_HTTP, "GET %s -> HTTP %d", url, status);
            err = ESP_FAIL;
        } else {
            *out = hb.buf;
            if (out_len) *out_len = hb.len;
            hb.buf = NULL; // transfer ownership
        }
    }
    esp_http_client_cleanup(c);
    free(hb.buf);
    return err;
}

esp_err_t http_put_string(const char *url, const char *payload, size_t len) {
    if (!url) return ESP_ERR_INVALID_ARG;

    esp_http_client_config_t cfg = {
        .url = url,
        .timeout_ms = 8000,
        .keep_alive_enable = true,
        .method = HTTP_METHOD_PUT,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    if (!c) return ESP_FAIL;

    if (payload && len > 0) esp_http_client_set_post_field(c, payload, len);

    esp_err_t err = esp_http_client_perform(c);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(c);
        if (status < 200 || status >= 300) {
            ESP_LOGE(TAG_HTTP, "PUT %s -> HTTP %d", url, status);
            err = ESP_FAIL;
        }
    }
    esp_http_client_cleanup(c);
    return err;
}
