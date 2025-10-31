#pragma once
#include <stdbool.h>
#include "core/app_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

// Baixa e armazena feriados em memória (ctx)
// Espera JSON tipo ["25-12","01-01",...]
void holidays_download_and_cache(app_ctx_t *ctx);

// Retorna true se (dia, mes) é feriado (usa cache do ctx)
bool holidays_is_holiday(int dia, int mes);

#ifdef __cplusplus
}
#endif
