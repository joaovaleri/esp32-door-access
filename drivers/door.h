#pragma once
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "core/config.h"

// Inicializa GPIOs da fechadura e LEDs
void door_init(void);

// Pulso curto na fechadura
void door_pulse(void);

// LEDs de feedback
void door_led_green_blink(void);
void door_led_red_blink(void);
