#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AGENDA_MAX_ITEMS  MAX_AGENDA   // herda do config.h

typedef struct {
  char hora[8];          // "HH:MM"
  int  estado;           // alvo [CONTADOR_MIN..CONTADOR_MAX]
  char tipo;             // 'D' (dias úteis) ou 'F' (fim de semana/feriado)
  char last_executed[8]; // "HH:MM" da última execução (resetado fora do horário)
} agenda_item_t;

typedef struct {
  agenda_item_t items[AGENDA_MAX_ITEMS];
  int count;
} agenda_list_t;

// zera lista
void agenda_reset(agenda_list_t *a);

// parse simples do JSON no formato {"HH:MM":"9D", "18:30":"1F", ...}
// retorna true se ao menos 1 item válido foi lido
bool agenda_parse_json_min(const char *json, agenda_list_t *out);

// decide se um item deve rodar no minuto atual
bool agenda_should_run(const agenda_item_t *it, const char *hora_atual, bool fds_ou_feriado);

#ifdef __cplusplus
}
#endif
