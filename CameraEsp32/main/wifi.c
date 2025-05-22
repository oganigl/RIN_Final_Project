#include "wifi.h"
static const char *ssid = "SAMSUNG";
static const char *pass = "12345678";
static const char* WIFI_TAG = "WIFI";
static int retry_num=0;
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(WIFI_TAG, "WIFI CONNECTING....");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(WIFI_TAG, "WiFi CONNECTED");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(WIFI_TAG, "WiFi lost connection");
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI(WIFI_TAG, "Retrying to Connect...");
        }
    }
    else if (event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(WIFI_TAG, "WiFi got IP...");
    }
}

void start_wifi() {
    // 2 - Wi-Fi Configuration Phase
    esp_netif_init();
    esp_event_loop_create_default();     // event loop
    esp_netif_create_default_wifi_sta(); // WiFi station
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "",
            .password = "",
        }
    };
    strcpy((char *)wifi_configuration.sta.ssid, ssid);
    strcpy((char *)wifi_configuration.sta.password, pass);

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    // 4 - Wi-Fi Connect Phase
    esp_wifi_connect();

    ESP_LOGI(WIFI_TAG, "wifi_init_softap finished. SSID:%s  password:%s", ssid, pass);
}
