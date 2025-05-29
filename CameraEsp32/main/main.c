#include <stdio.h>
#include "wifi.h"
#include "mqtt.h"
#include "camera.h"
const char* FLASH_TAG = "FLASH";
const char* Publisher_topic = "abrir_puerta/camara";
void take_and_send_photo_callback(const char* topic, const char* data){
    ESP_LOGI("MQTT","Hemos recibido el callback GG");
    camera_fb_t *image = camera_capture();
    if(!image){
         ESP_LOGI(CAMERA_TAG,"Error al tomar la imagen");
         return;
    }
    if(send_message(Publisher_topic,(char*)(image->buf),image->len) < 0){
        ESP_LOGE(MQTT_TAG,"ERROR AL ENVIAR EL MENSAJE");
    }
    esp_camera_fb_return(image);
}
void app_main(void)
{   

    static const mqtt_topic_handler_t topic_handlers[] = {
        { .topic = "abrir_puerta/identificador", .callback = take_and_send_photo_callback}
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
        ESP_LOGE(CAMERA_TAG, "Error al inicializar la cámara");
        return;
    }
    ESP_LOGI(CAMERA_TAG, "Cámara inicializada correctamente");

    // Conectarse a WiFi
    start_wifi(); 
    // Iniciar MQTT
     ESP_LOGI("HEAP", "Memoria libre antes de MQTT: %d bytes",(int)esp_get_free_heap_size());
    mqtt_start(topic_handlers, 1);
    ESP_LOGI(MQTT_TAG, "MQTT inicializado y suscrito a los temas");
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Bucle pasivo que evita que app_main termine
    }

}
   