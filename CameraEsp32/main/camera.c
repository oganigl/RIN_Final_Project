#include "camera.h"
const char* CAMERA_TAG = "CAMERA";
esp_err_t camera_init(){
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
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
    //replace this with your own function
    camera_fb_t *image = malloc(sizeof(camera_fb_t));
    *image = *fb;
    image->buf = (uint8_t*)malloc(fb->len);
    if(!image->buf){
        ESP_LOGE(CAMERA_TAG, "No memory for image buffer");
        free(image);
        return NULL;
    }
    //return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
    return image;
}