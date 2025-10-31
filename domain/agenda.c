#include "domain/agenda.h"
#include "core/config.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void agenda_reset(agenda_list_t *a) {
  if (!a) return;
  memset(a, 0, sizeof(*a));
}

static void trim_copy(char *dst, size_t dstsz, const char *src, int len) {
  if (!dst || !src || dstsz==0 || len<=0) { if (dst && dstsz) dst[0]=0; return; }
  int n = (len < (int)dstsz-1) ? len : (int)dstsz-1;
  memcpy(dst, src, n);
  dst[n] = 0;
}

bool agenda_parse_json_min(const char *json, agenda_list_t *out) {
  if (!json || !out) return false;
  agenda_reset(out);

  const char *p = json;
  while (out->count < AGENDA_MAX_ITEMS) {
    // chave "HH:MM"
    const char *k1 = strchr(p, '\"'); if (!k1) break;
    const char *k2 = strchr(k1+1, '\"'); if (!k2) break;

    // dois pontos após a chave
    const char *colon = strchr(k2, ':'); if (!colon) break;

    // pula espaços
    const char *v = colon + 1;
    while (*v==' ' || *v=='\t') v++;

    // valor deve iniciar com aspas
    if (*v != '\"') { p = k2 + 1; continue; }
    const char *v1 = v + 1;
    const char *v2 = strchr(v1, '\"'); if (!v2) break;

    // extrai chave e valor
    int hlen = (int)(k2 - k1 - 1);
    int vlen = (int)(v2 - v1);

    if (hlen > 0 && hlen < 8 && vlen > 0 && vlen < 8) {
      agenda_item_t *it = &out->items[out->count];
      memset(it, 0, sizeof(*it));

      // hora "HH:MM"
      trim_copy(it->hora, sizeof(it->hora), k1+1, hlen);

      // valor: "9D" ou "10F" (estado + tipo)
      char valor[8] = {0};
      trim_copy(valor, sizeof(valor), v1, vlen);

      // parse estado e tipo (último char é tipo, anterior(-es) é número)
      int L = strlen(valor);
      char tipo = (L > 0) ? valor[L-1] : 'D';
      valor[L > 0 ? L-1 : 0] = 0; // remove o tipo
      int estado = atoi(valor);

      // saneamento
      if (estado < CONTADOR_MIN || estado > CONTADOR_MAX) {
        estado = CONTADOR_MIN;
      }
      if (tipo != 'D' && tipo != 'F') tipo = 'D';

      it->estado = estado;
      it->tipo   = tipo;
      it->last_executed[0] = 0;

      out->count++;
    }

    // segue após o valor atual
    p = v2 + 1;
  }

  return out->count > 0;
}

bool agenda_should_run(const agenda_item_t *it, const char *hora_atual, bool fds_ou_feriado) {
  if (!it || !hora_atual) return false;

  // Regra de dia:
  // - 'D' roda só em dias úteis (não feriado)
  // - 'F' roda só em fim de semana OU feriado
  bool dia_ok = ( (it->tipo=='D' && !fds_ou_feriado) ||
                  (it->tipo=='F' &&  fds_ou_feriado) );

  if (!dia_ok) return false;

  // roda 1x por minuto: hora coincide e ainda não executou neste minuto
  return strcmp(hora_atual, it->hora) == 0 &&
         strcmp(hora_atual, it->last_executed) != 0;
}
