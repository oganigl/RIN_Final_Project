#include "wifi.h"
#include "freertos/event_groups.h"

static const char *ssid = "SAMSUNG";
static const char *pass = "12345678";
static const char* WIFI_TAG = "WIFI";

static int retry_num = 0;
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

// Handler de eventos
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                ESP_LOGI(WIFI_TAG, "WIFI CONNECTING....");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(WIFI_TAG, "WiFi CONNECTED");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(WIFI_TAG, "WiFi lost connection");
                if (retry_num < 5) {
                    esp_wifi_connect();
                    retry_num++;
                    ESP_LOGI(WIFI_TAG, "Retrying to Connect...");
                }
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(WIFI_TAG, "WiFi got IP...");
        retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void start_wifi() {
    wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

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

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_start();

    ESP_LOGI(WIFI_TAG, "WiFi started, waiting for connection...");

    // Espera bloqueante hasta que obtenga IP
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                                           pdFALSE, pdTRUE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFI_TAG, "WiFi connected successfully. IP assigned.");
    }
}
