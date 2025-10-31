#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "core/config.h"
#include "domain/agenda.h" 

// Para armazenar cartões autorizados sem depender de todo o header do RC522,
// definimos um tipo de UID mínimo compatível (length + até 10 bytes).
// (Se preferir usar o tipo nativo, troque por: #include "rc522_types.h")
typedef struct {
    uint8_t length;
    uint8_t value[10];
} uid_t;

// Ponteiros opacos para drivers (evita incluir headers de drivers no core)
typedef void* rfid_driver_handle_t;
typedef void* rfid_scanner_handle_t;

typedef struct {
    // --- Estado lógico da aplicação ---
    int   counter;             // posição/estado atual
    bool  developer_mode;      // modo desenvolvedor (toggle pela master key)

    // --- Wi-Fi / IP ---
    EventGroupHandle_t wifi_event_group; // opcional: usado por net/wifi_enterprise.c

    // --- RFID (handles opacos) ---
    rfid_driver_handle_t rfid_driver;
    rfid_scanner_handle_t rfid_scanner;

    // --- Cartões autorizados persistidos ---
    uid_t   authorized_cards[MAX_AUTHORIZED_CARDS];
    uint8_t authorized_count;

    // --- Agenda em memória ---
    agenda_list_t agenda;  // <--- NOVO CAMPO PARA AS TASKS USAREM
    
    // --- Janela de bloqueio de pulso manual (anti-bounce lógico) ---
    uint64_t bloqueia_pulso_ate_us;
} app_ctx_t;

// Inicializa com defaults seguros (zera tudo e aplica mínimos)
void app_ctx_init(app_ctx_t *ctx);
