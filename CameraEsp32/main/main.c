#include <stdio.h>
#include "wifi.h"
#include "mqtt.h"
#include "camera.h"
const char* FLASH_TAG = "FLASH";
const char* Publisher_topic = "abrir_puerta/camara";
void take_and_send_photo_callback(const char* topic, const char* data){
    camera_fb_t *image = camera_capture();
    if(!image){
         ESP_LOGI(CAMERA_TAG,"Error al tomar la imagen");
         return;
    }
    ESP_LOGI(CAMERA_TAG,"Capturado imagen, vamos a enviarla por mqtt");
    if(send_message(Publisher_topic,(char*)(image->buf),image->len) < 0){
        ESP_LOGE(MQTT_TAG,"ERROR AL ENVIAR EL MENSAJE");
    }
    free(image->buf);
    free(image);
}
void app_main(void)
{   
    static const mqtt_topic_handler_t topic_handlers[] = {
        {"abrir_puerta/identificador", take_and_send_photo_callback}
    };
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NO_FREE_PAGES && ret != ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(FLASH_TAG, "Error al inicializar NVS: %s", esp_err_to_name(ret));
        return;
    }
    if (ret != ESP_OK) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(FLASH_TAG, "NVS inicializado correctamente");

    // Inicializar cámara
    if (camera_init() != ESP_OK) {
        return;
    }
    ESP_LOGI(CAMERA_TAG, "Cámara inicializada correctamente");

    // Conectarse a WiFi
    start_wifi(); // Asegúrate que esta función bloquea hasta conexión establecida

    // Iniciar MQTT
    mqtt_start("camera", "1234", topic_handlers, sizeof(topic_handlers) / sizeof(topic_handlers[0]));
    ESP_LOGI(MQTT_TAG, "MQTT inicializado y suscrito a los temas");
}
   