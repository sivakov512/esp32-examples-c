#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include <stdio.h>

#define LED_PIN 1
#define BUTTON_PIN 0

void button_task(void *pvParam) {
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << BUTTON_PIN),
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = 1,
                           .pull_down_en = 0,
                           .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&io_conf);

  while (1) {
    int btn_level = gpio_get_level(BUTTON_PIN);
    gpio_set_level(LED_PIN, !btn_level);
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void app_main(void) {
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << LED_PIN),
                           .mode = GPIO_MODE_OUTPUT,
                           .pull_up_en = 0,
                           .pull_down_en = 0,
                           .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&io_conf);

  xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);
}
