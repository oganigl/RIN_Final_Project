#ifndef CAMERA_H
#define CAMERA_H

#include "esp_camera.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

// ===================
// Select camera model
// ===================
#define LILYGO_ESP32S3_CAM_PIR_VOICE // Has PSRAM

// ===================
// Set this to true if using AP mode
// ===================
#define USING_AP_MODE true

// ===================
// Pins
// ===================
#ifdef I2C_SDA
#undef I2C_SDA
#endif

#ifdef I2C_SCL
#undef I2C_SCL
#endif

#if defined(LILYGO_ESP32S3_CAM_PIR_VOICE)

#define PWDN_GPIO_NUM               (-1)
#define RESET_GPIO_NUM              (39)
#define XCLK_GPIO_NUM               (38)
#define SIOD_GPIO_NUM               (5)
#define SIOC_GPIO_NUM               (4)
#define VSYNC_GPIO_NUM              (8)
#define HREF_GPIO_NUM               (18)
#define PCLK_GPIO_NUM               (12)
#define Y9_GPIO_NUM                 (9)
#define Y8_GPIO_NUM                 (10)
#define Y7_GPIO_NUM                 (11)
#define Y6_GPIO_NUM                 (13)
#define Y5_GPIO_NUM                 (21)
#define Y4_GPIO_NUM                 (48)
#define Y3_GPIO_NUM                 (47)
#define Y2_GPIO_NUM                 (14)

#define I2C_SDA                     (7)
#define I2C_SCL                     (6)

#define PIR_INPUT_PIN               (17)
#define PMU_INPUT_PIN               (2)

#define IIS_WS_PIN                  (42)
#define IIS_DIN_PIN                 (41)
#define IIS_SCLK_PIN                (40)

#define EXTERN_PIN1                 (16)
#define EXTERN_PIN2                 (15)

#define BUTTON_CONUT                (1)
#define USER_BUTTON_PIN             (0)
#define BUTTON_ARRAY                {USER_BUTTON_PIN}

#elif defined(LILYGO_ESP32S3_CAM_SIM7080G)

#define PWDN_GPIO_NUM               (-1)
#define RESET_GPIO_NUM              (18)
#define XCLK_GPIO_NUM               (8)
#define SIOD_GPIO_NUM               (2)
#define SIOC_GPIO_NUM               (1)
#define VSYNC_GPIO_NUM              (16)
#define HREF_GPIO_NUM               (17)
#define PCLK_GPIO_NUM               (12)
#define Y9_GPIO_NUM                 (9)
#define Y8_GPIO_NUM                 (10)
#define Y7_GPIO_NUM                 (11)
#define Y6_GPIO_NUM                 (13)
#define Y5_GPIO_NUM                 (21)
#define Y4_GPIO_NUM                 (48)
#define Y3_GPIO_NUM                 (47)
#define Y2_GPIO_NUM                 (14)

#define I2C_SDA                     (15)
#define I2C_SCL                     (7)

#define PMU_INPUT_PIN               (6)

#define BUTTON_CONUT                (1)
#define USER_BUTTON_PIN             (0)
#define BUTTON_ARRAY                {USER_BUTTON_PIN}

#define BOARD_MODEM_PWR_PIN         (41)
#define BOARD_MODEM_DTR_PIN         (42)
#define BOARD_MODEM_RI_PIN          (3)
#define BOARD_MODEM_RXD_PIN         (4)
#define BOARD_MODEM_TXD_PIN         (5)

#define USING_MODEM

#else
#error "Camera model not selected"
#endif

// Map pin names to generic defines used by camera_config
#define CAM_PIN_PWDN                PWDN_GPIO_NUM
#define CAM_PIN_RESET               RESET_GPIO_NUM
#define CAM_PIN_XCLK                XCLK_GPIO_NUM
#define CAM_PIN_SIOD                SIOD_GPIO_NUM
#define CAM_PIN_SIOC                SIOC_GPIO_NUM

#define CAM_PIN_D7                  Y9_GPIO_NUM
#define CAM_PIN_D6                  Y8_GPIO_NUM
#define CAM_PIN_D5                  Y7_GPIO_NUM
#define CAM_PIN_D4                  Y6_GPIO_NUM
#define CAM_PIN_D3                  Y5_GPIO_NUM
#define CAM_PIN_D2                  Y4_GPIO_NUM
#define CAM_PIN_D1                  Y3_GPIO_NUM
#define CAM_PIN_D0                  Y2_GPIO_NUM

#define CAM_PIN_VSYNC              VSYNC_GPIO_NUM
#define CAM_PIN_HREF               HREF_GPIO_NUM
#define CAM_PIN_PCLK               PCLK_GPIO_NUM

extern const char* CAMERA_TAG;

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_UXGA,
    .jpeg_quality = 12,
    .fb_count = 1,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

esp_err_t camera_init();
camera_fb_t* camera_capture();

#endif // CAMERA_H
