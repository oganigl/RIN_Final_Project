#ifndef CAMERA_H
#define CAMERA_H

#include "esp_camera.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "camera_pin.h"

extern const char* CAMERA_TAG;

esp_err_t camera_init();
camera_fb_t* camera_capture();

#endif // CAMERA_H
