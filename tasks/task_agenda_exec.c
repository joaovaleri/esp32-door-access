#include "tasks/task_agenda_exec.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/config.h"
#include "core/log_tags.h"
#include "drivers/door.h"
#include "net/http_simple.h"
#include "storage/counter_store.h"
#include "domain/agenda.h"
#include "domain/holidays.h"
#include "esp_log.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

static void _push_counter_cloud(int value) {
    char body[12];
    int n = snprintf(body, sizeof(body), "%d", value);
    if (n > 0) (void)http_put_string(COUNTER_URL, body, (size_t)n);
}

static bool _is_weekend_or_holiday(void) {
    time_t now; struct tm t; time(&now); localtime_r(&now, &t);
    int dow = t.tm_wday; // 0=domingo, 6=sábado
    if (dow == 0 || dow == 6) return true;
    return holidays_is_holiday(t.tm_mday, t.tm_mon + 1);
}

static void _hhmm_now(char out[8]) {
    time_t now; struct tm t; time(&now); localtime_r(&now, &t);
    strftime(out, 8, "%H:%M", &t);
}

static void task(void *arg) {
    app_ctx_t *ctx = (app_ctx_t*)arg;

    while (1) {
        char agora[8]; _hhmm_now(agora);
        bool fds_fer = _is_weekend_or_holiday();

        for (int i = 0; i < ctx->agenda.count; ++i) {
            agenda_item_t *it = &ctx->agenda.items[i];
            if (agenda_should_run(it, agora, fds_fer)) {
                int alvo = it->estado;
                int atual = ctx->counter;

                ESP_LOGI(TAG_AGENDA, "Executando %s -> alvo %d (atual %d)", it->hora, alvo, atual);

                if (alvo != atual) {
                    int pulsos = (alvo - atual + (CONTADOR_MAX - CONTADOR_MIN + 1)) % (CONTADOR_MAX - CONTADOR_MIN + 1);
                    if (pulsos == 0) pulsos = CONTADOR_MAX - CONTADOR_MIN + 1;

                    for (int p = 0; p < pulsos; ++p) {
                        door_pulse();
                        ctx->counter = counter_next(ctx->counter);
                        vTaskDelay(pdMS_TO_TICKS(200));
                    }
                    (void)counter_save(ctx->counter);
                    _push_counter_cloud(ctx->counter);
                }
                strncpy(it->last_executed, agora, sizeof(it->last_executed));
            }

            // se a hora atual mudou, liberamos o gatilho para o próximo dia/minuto
            if (strcmp(agora, it->hora) != 0) {
                it->last_executed[0] = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void start_task_agenda_exec(app_ctx_t *ctx) {
    xTaskCreate(task, "task_agenda_exec", 4096, ctx, 3, NULL);
}
