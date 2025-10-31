#include "domain/holidays.h"
#include "net/http_simple.h"
#include "core/config.h"
#include "core/log_tags.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Buffer local (em memória do módulo) dos feriados
static char s_feriados[MAX_FERIADOS][6]; // "DD-MM"
static int  s_feriados_count = 0;

static void _reset_cache(void) {
  for (int i = 0; i < MAX_FERIADOS; ++i) s_feriados[i][0] = 0;
  s_feriados_count = 0;
}

static void _trim_copy(char *dst, size_t dstsz, const char *src, int len) {
  if (!dst || !src || dstsz==0 || len<=0) { if (dst && dstsz) dst[0]=0; return; }
  int n = (len < (int)dstsz-1) ? len : (int)dstsz-1;
  memcpy(dst, src, n);
  dst[n] = 0;
}

// Parse de ["25-12","01-01", ...]
static int _parse_list_ddmm(const char *json) {
  if (!json) return 0;
  int count = 0;
  const char *p = json;

  while (count < MAX_FERIADOS) {
    const char *q1 = strchr(p, '\"');
    if (!q1) break;
    const char *q2 = strchr(q1+1, '\"');
    if (!q2) break;

    int len = (int)(q2 - q1 - 1);
    if (len > 0 && len < 6) {
      _trim_copy(s_feriados[count], sizeof(s_feriados[count]), q1+1, len);
      count++;
    }
    p = q2 + 1;
  }
  return count;
}

void holidays_download_and_cache(app_ctx_t *ctx) {
  (void)ctx; // não usamos ctx diretamente, mantemos cache local
  _reset_cache();

  char *resp = NULL; size_t len = 0;
  if (http_get_alloc(FERIADOS_URL, &resp, &len) == ESP_OK && resp) {
    int got = _parse_list_ddmm(resp);
    s_feriados_count = got;
    ESP_LOGI(TAG_HOLIDAYS, "Feriados baixados: %d entradas", got);
  } else {
    ESP_LOGW(TAG_HOLIDAYS, "Falha ao baixar feriados.");
  }
  free(resp);
}

bool holidays_is_holiday(int dia, int mes) {
  char key[6];
  snprintf(key, sizeof(key), "%02d-%02d", dia, mes);
  for (int i = 0; i < s_feriados_count; ++i) {
    if (strcmp(s_feriados[i], key) == 0) return true;
  }
  return false;
}
