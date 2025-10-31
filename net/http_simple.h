#pragma once
#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// GET que aloca buffer e devolve em *out (caller dá free()).
esp_err_t http_get_alloc(const char *url, char **out, size_t *out_len);

// PUT simples de payload texto/binário (sem headers custom).
esp_err_t http_put_string(const char *url, const char *payload, size_t len);

#ifdef __cplusplus
}
#endif
