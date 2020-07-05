/* Wi-Fi Provisioning Manager Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "ClockWidget.h"
#include "Effectwidget.h"
#include "buttons.h"
#include "display.h"
#include "network.h"

static const char *TIMEZONE = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";

static const char *TAG = "main";

static void initFlash(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  }
}

static void initTime(void) {
  setenv("TZ", TIMEZONE, 1);
  tzset();
}

extern "C" void app_main(void) {
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  buttons::init();

  ClockWidget clockWidget;
  EffectWidget effectWidget(clockWidget);
  display::init(&effectWidget);
  initFlash();

  initTime();
  network::init();

  /* Start main application now */
  while (1) {
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];
    time(&now);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "Time is: %s", strftime_buf);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
