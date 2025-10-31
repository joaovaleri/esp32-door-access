#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

int  counter_load_or_default(void);     // retorna CONTADOR_MIN se não existir
esp_err_t counter_save(int v);          // salva no NVS
int  counter_next(int current);         // avança circular

#ifdef __cplusplus
}
#endif
