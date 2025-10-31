#include "drivers/door.h"
#include "esp_log.h"
#include "core/log_tags.h"

void door_init(void) {
    ESP_LOGI(TAG_DOOR, "Inicializando GPIOs da porta e LEDs...");

    gpio_set_direction(DOOR_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_RED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(DOOR_GPIO, 0);
    gpio_set_level(LED_RED_GPIO, 0);
    gpio_set_level(LED_GREEN_GPIO, 0);
}

void door_pulse(void) {
    ESP_LOGI(TAG_DOOR, "Pulso enviado à fechadura");
    gpio_set_level(DOOR_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(100));   // pulso de 100 ms
    gpio_set_level(DOOR_GPIO, 0);
}

static void blink(gpio_num_t pin) {
    gpio_set_level(pin, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(pin, 0);
}

void door_led_green_blink(void) { blink(LED_GREEN_GPIO); }
void door_led_red_blink(void)   { blink(LED_RED_GPIO);   }
