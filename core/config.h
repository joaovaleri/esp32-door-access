#pragma once
#include "driver/gpio.h"

// ====== Valores vindos do menuconfig (sdkconfig) ======
#define WIFI_SSID        CONFIG_WIFI_SSID
#define EAP_IDENTITY     CONFIG_EAP_IDENTITY
#define EAP_USERNAME     CONFIG_EAP_USERNAME
#define EAP_PASSWORD     CONFIG_EAP_PASSWORD

#define TARGET_URL       CONFIG_TARGET_URL
#define COUNTER_URL      CONFIG_COUNTER_URL
#define AGENDA_URL       CONFIG_AGENDA_URL
#define RESET_URL        CONFIG_RESET_URL
#define FERIADOS_URL     CONFIG_FERIADOS_URL
#define MASTER_KEY_HASH  CONFIG_MASTER_KEY_HASH

// ====== GPIOs de hardware ======
#define DOOR_GPIO        GPIO_NUM_27
#define LED_RED_GPIO     GPIO_NUM_2
#define LED_GREEN_GPIO   GPIO_NUM_4

// ====== Limites de domínio ======
#define CONTADOR_MIN           1
#define CONTADOR_MAX           10
#define MAX_AUTHORIZED_CARDS   20
#define MAX_AGENDA             10
#define MAX_FERIADOS           64

// ====== NVS ======
#define NVS_NS            "storage"
#define NVS_KEY_COUNTER   "counter"
#define NVS_KEY_CARDS     "cards"
#define NVS_KEY_CARD_CNT  "card_count"

// ====== Outros ======
#define BLOQUEIO_PULSO_US (5ULL * 1000000ULL)  // 5s em micros
#define DEFAULT_TZ        "GMT+3"              // America/Sao_Paulo aproximado
