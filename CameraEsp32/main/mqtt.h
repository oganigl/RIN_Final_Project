
#ifndef MQTT_H
#define MQTT_H
#include "mqtt_client.h"
#include "esp_event.h" //managing events of mqtt
#include "nvs_flash.h" //storing mqtt and wifi configs and settings
#include "freertos/FreeRTOS.h" //it is important too if you want to run mqtt task independently and provides threads funtionality
#include "freertos/task.h" //MQTT communication often involves asynchronous operations, and FreeRTOS helps handle those tasks effectively
#include "esp_log.h" //log out put, do not use printf everywhere
extern const char* MQTT_TAG;
typedef void (*mqtt_topic_callback_t)(const char *topic,const char *data);
typedef struct{
    const char *topic;
    mqtt_topic_callback_t callback;
} mqtt_topic_handler_t;

void  mqtt_start(char* username,char* password, const mqtt_topic_handler_t topic_handlers[],int num_handlers);
int send_message(const char* topic,char* data,int len);
#endif