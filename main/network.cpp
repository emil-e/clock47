#include "network.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include <esp_sntp.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

static const char *TAG = "network";

namespace network {
namespace {

/* Signal Wi-Fi events on this event-group */
const int WIFI_CONNECTED_EVENT = BIT0;
EventGroupHandle_t wifi_event_group;

/* Event handler for catching system events */
void eventHandler(void *arg, esp_event_base_t event_base, int event_id, void *event_data) {
  if (event_base == WIFI_PROV_EVENT) {
    switch (event_id) {
    case WIFI_PROV_START:
      ESP_LOGI(TAG, "Provisioning started");
      break;
    case WIFI_PROV_CRED_RECV: {
      wifi_sta_config_t *wifi_sta_cfg = (wifi_sta_config_t *)event_data;
      ESP_LOGI(TAG,
               "Received Wi-Fi credentials"
               "\n\tSSID     : %s\n\tPassword : %s",
               (const char *)wifi_sta_cfg->ssid, (const char *)wifi_sta_cfg->password);
      break;
    }
    case WIFI_PROV_CRED_FAIL: {
      wifi_prov_sta_fail_reason_t *reason = (wifi_prov_sta_fail_reason_t *)event_data;
      ESP_LOGE(TAG,
               "Provisioning failed!\n\tReason : %s"
               "\n\tPlease reset to factory and retry provisioning",
               (*reason == WIFI_PROV_STA_AUTH_ERROR) ? "Wi-Fi station authentication failed"
                                                     : "Wi-Fi access-point not found");
      break;
    }
    case WIFI_PROV_CRED_SUCCESS:
      ESP_LOGI(TAG, "Provisioning successful");
      break;
    case WIFI_PROV_END:
      /* De-initialize manager once provisioning is finished */
      wifi_prov_mgr_deinit();
      break;
    default:
      break;
    }
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
    /* Signal main application to continue execution */
    xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_EVENT);
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
    esp_wifi_connect();
  }
}

void timeSyncNotificationCallback(struct timeval *tv) {
  ESP_LOGI(TAG, "Time synchronized: %ld", tv->tv_sec);
}

void wifiInitSta() {
  /* Start Wi-Fi in station mode */
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void getDeviceServiceName(char *service_name, size_t max) {
  uint8_t eth_mac[6];
  const char *ssid_prefix = "clock47_";
  esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
  snprintf(service_name, max, "%s%02X%02X%02X", ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);
}

void initWifi() {
  ESP_ERROR_CHECK(esp_netif_init());

  wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(
      esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &eventHandler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler, NULL));

  esp_netif_create_default_wifi_sta();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  wifi_prov_mgr_config_t config = {.scheme = wifi_prov_scheme_ble,
                                   .scheme_event_handler =
                                       WIFI_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM};

  ESP_ERROR_CHECK(wifi_prov_mgr_init(config));

  bool provisioned = false;
  ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));

  if (!provisioned) {
    ESP_LOGI(TAG, "Starting provisioning");
    char service_name[12];
    getDeviceServiceName(service_name, sizeof(service_name));
    wifi_prov_security_t security = WIFI_PROV_SECURITY_1;

    const char *pop = NULL;
    const char *service_key = NULL;

    // This step is only useful when scheme is wifi_prov_scheme_ble. This will
    // set a custom 128 bit UUID which will be included in the BLE advertisement
    // and will correspond to the primary GATT service that provides
    // provisioning endpoints as GATT characteristics. Each GATT characteristic
    // will be formed using the primary service UUID as base, with different
    // auto assigned 12th and 13th bytes (assume counting starts from 0th byte).
    // The client side applications must identify the endpoints by reading the
    // User Characteristic Description descriptor (0x2901) for each
    // characteristic, which contains the endpoint name of the characteristic
    uint8_t custom_service_uuid[] = {
        /* LSB <---------------------------------------
         * ---------------------------------------> MSB */
        0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
        0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
    };
    wifi_prov_scheme_ble_set_service_uuid(custom_service_uuid);

    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(security, pop, service_name, service_key));
  } else {
    ESP_LOGI(TAG, "Already provisioned, starting Wi-Fi STA");
    wifi_prov_mgr_deinit();
    wifiInitSta();
  }

  /* Wait for Wi-Fi connection */
  xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_EVENT, false, true, portMAX_DELAY);
}

void initSntp() {
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "pool.ntp.org");
  sntp_set_time_sync_notification_cb(timeSyncNotificationCallback);
  sntp_init();
}

} // namespace

void init() {
  initWifi();
  initSntp();
}

} // namespace network
