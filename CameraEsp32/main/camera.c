#include "camera.h"
#include "mqtt.h"
const char* CAMERA_TAG = "CAMERA";

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 15
#define SIOD_GPIO_NUM 4
#define SIOC_GPIO_NUM 5

#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM 7
#define PCLK_GPIO_NUM 13


camera_config_t config;

esp_err_t camera_init(){

    //initialize the camera

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_SVGA;
    config.pixel_format = PIXFORMAT_JPEG; 
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;

    
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(CAMERA_TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}


camera_fb_t* camera_capture(){
    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(CAMERA_TAG, "Camera Capture Failed");
        return NULL;
    }
    ESP_LOGI(CAMERA_TAG, "fbn capturada:");
    ESP_LOGI(CAMERA_TAG, " - Ancho: %d px", fb->width);
    ESP_LOGI(CAMERA_TAG, " - Alto: %d px", fb->height);
    ESP_LOGI(CAMERA_TAG, " - Tamaño: %d bytes", fb->len);
    switch (fb->format) {
        case PIXFORMAT_JPEG:
            ESP_LOGI(CAMERA_TAG, " - Formato: JPEG");
            break;
        case PIXFORMAT_RGB565:
            ESP_LOGI(CAMERA_TAG, " - Formato: RGB565");
            break;
        case PIXFORMAT_YUV422:
            ESP_LOGI(CAMERA_TAG, " - Formato: YUV422");
            break;
        case PIXFORMAT_GRAYSCALE:
            ESP_LOGI(CAMERA_TAG, " - Formato: Grayscale");
            break;
        case PIXFORMAT_RGB888:
            ESP_LOGI(CAMERA_TAG, " - Formato: RGB888");
            break;
        default:
            ESP_LOGI(CAMERA_TAG, " - Formato desconocido (%d)", fb->format);
            break;
    }
    ESP_LOGI(CAMERA_TAG,"Capturado imagen, vamos a enviarla por mqtt");
    // if(send_message("abrir_puerta/camara",(char*)(fb->buf),fb->len) < 0){
    //     ESP_LOGE(MQTT_TAG,"ERROR AL ENVIAR EL MENSAJE");
    // }
    return fb;
}