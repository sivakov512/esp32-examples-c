#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "portmacro.h"
#include <stdint.h>
#include <stdio.h>

#define BLINK_PERIOD_MS 1000
#define LED_PIN 1

static TimerHandle_t blink_timer;
static bool led_on = false;

static void blink_timer_cb(TimerHandle_t xTimer) {
  led_on = !led_on;
  gpio_set_level(LED_PIN, led_on ? 1 : 0);
}

void app_main(void) {
  gpio_config_t led_conf = {.pin_bit_mask = (1ULL << LED_PIN),
                            .mode = GPIO_MODE_OUTPUT,
                            .pull_up_en = 0,
                            .pull_down_en = 0,
                            .intr_type = GPIO_INTR_DISABLE};
  gpio_config(&led_conf);

  blink_timer = xTimerCreate("blink_timer", pdMS_TO_TICKS(BLINK_PERIOD_MS),
                             pdTRUE, NULL, blink_timer_cb);
  xTimerStart(blink_timer, 0);

  while (1) {
    vTaskDelay(portMAX_DELAY);
  }
}
