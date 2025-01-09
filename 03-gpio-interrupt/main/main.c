#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>

#define LED_PIN 1
#define BUTTON_PIN 0

static void IRAM_ATTR gpio_isr_handler(void *arg) {
  uint32_t gpio_num = (uint32_t)arg;

  int btn_level = gpio_get_level(gpio_num);
  gpio_set_level(LED_PIN, !btn_level);
}

void app_main(void) {
  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << BUTTON_PIN),
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = 1,
                           .pull_down_en = 0,
                           .intr_type = GPIO_INTR_ANYEDGE};
  gpio_config(&io_conf);

  gpio_config_t led_conf = {.pin_bit_mask = (1ULL << LED_PIN),
                            .mode = GPIO_MODE_OUTPUT,
                            .pull_up_en = 0,
                            .pull_down_en = 0,
                            .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&led_conf);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)BUTTON_PIN);
}
