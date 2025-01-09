#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"

#include "wifi_manager.h"

static const char *TAG = "MAIN";

#define SSID "sivakov512-wifi"
#define PASSWD "singularity"

static volatile bool s_button_pressed = false;
#define BUTTON_PIN 0

static void IRAM_ATTR gpio_isr_handler(void *arg) { s_button_pressed = true; }

void http_request_task(void *pvParam) {
  while (1) {
    if (s_button_pressed) {
      s_button_pressed = false;

      esp_http_client_config_t config = {.url = "http://httpbin.org/get"};
      esp_http_client_handle_t client = esp_http_client_init(&config);

      esp_err_t err = esp_http_client_perform(client);
      if (err == ESP_OK) {
        ESP_LOGI(TAG, "Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
      } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
      }
      esp_http_client_cleanup(client);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void app_main(void) {
  esp_err_t err = wifi_init_sta(SSID, PASSWD);
  if (err == ESP_OK) {
    ESP_LOGI(TAG, "Wifi connected successfully!");
  } else {
    ESP_LOGE(TAG, "WiFi connection failed with error: %d", err);
  }

  xTaskCreate(http_request_task, "http_request_task", 4096, NULL, 5, NULL);

  gpio_config_t io_conf = {.pin_bit_mask = (1ULL << BUTTON_PIN),
                           .mode = GPIO_MODE_INPUT,
                           .pull_up_en = 1,
                           .pull_down_en = 0,
                           .intr_type = GPIO_INTR_NEGEDGE};
  gpio_config(&io_conf);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)BUTTON_PIN);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
