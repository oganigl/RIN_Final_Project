#include "mqtt.h"
const char* MQTT_TAG = "MQTT";
static const mqtt_topic_handler_t *g_topic_handlers = NULL;
static int g_num_topic_handlers = 0;
static esp_mqtt_client_handle_t client;
static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
   esp_mqtt_event_handle_t event = event_data;
   esp_mqtt_client_handle_t client = event->client;
    if(event_id == MQTT_EVENT_CONNECTED){
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
        subscribe();
        }
     else if(event_id == MQTT_EVENT_DISCONNECTED)
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED"); 
      }
      else if(event_id == MQTT_EVENT_SUBSCRIBED)
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED");
      }
      else if(event_id == MQTT_EVENT_UNSUBSCRIBED) 
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED");
      }
      else if(event_id == MQTT_EVENT_DATA){
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
         char topic[event->topic_len+1];
         char data[event->data_len +1];
         memcpy(topic, event->topic, event->topic_len);
         topic[event->topic_len] = '\0';

         memcpy(data, event->data, event->data_len);
         data[event->data_len] = '\0';

         for(int i = 0; i < g_num_topic_handlers; i++){
            if(strcmp(g_topic_handlers[i].topic,topic) == 0){
               g_topic_handlers[i].callback(topic,data);
               break;
            }
         }
      }
    else if (event_id == MQTT_EVENT_ERROR) {
    ESP_LOGE(MQTT_TAG, "MQTT_EVENT_ERROR");
        ESP_LOGE(MQTT_TAG, "error_type: %d", event->error_handle->error_type);
        ESP_LOGE(MQTT_TAG, "esp_tls_last_esp_err: 0x%x", event->error_handle->esp_tls_last_esp_err);
        ESP_LOGE(MQTT_TAG, "esp_tls_stack_err: 0x%x", event->error_handle->esp_tls_stack_err);
        ESP_LOGE(MQTT_TAG, "sock errno: %d (%s)", event->error_handle->esp_transport_sock_errno,
                 strerror(event->error_handle->esp_transport_sock_errno));
    // Context info
    ESP_LOGI(MQTT_TAG, "Client handle: %p", event->client);
    ESP_LOGI(MQTT_TAG, "Event ID: %d", event->event_id);
}
}

void mqtt_start(const mqtt_topic_handler_t topic_handlers[],int num_handlers)
{/*Depending on your website or cloud there could be more parameters in mqtt_cfg.*/ 
   g_topic_handlers = topic_handlers;
  g_num_topic_handlers = num_handlers;
   const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.7.24:1883",
        .credentials.client_id = "ESP32_A5",
    };
  
   client = esp_mqtt_client_init(&mqtt_cfg); //sending struct as a parameter in init client function
   if (client == NULL) {
         ESP_LOGE(MQTT_TAG, "Error al inicializar el cliente MQTT");
         return;
      }
  esp_mqtt_client_register_event(client,ESP_EVENT_ANY_ID,mqtt_event_handler,NULL);
  
 
  esp_err_t err = esp_mqtt_client_start(client);
    if (err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "Error al iniciar el cliente MQTT: %s", esp_err_to_name(err));
    }
}
  int send_message(const char* topic,char* data,int len){
   return esp_mqtt_client_publish(client,topic,data,len,2,false);
   
  }
  void subscribe(){
   for(int i = 0; i < g_num_topic_handlers;i++){
            if(esp_mqtt_client_subscribe(client, g_topic_handlers[i].topic, 2) != -1) //QOs == 2 Debido a que queremos asegurarnos que los mensajes lleguen exactamente una vez
            {
               ESP_LOGI(MQTT_TAG,"Subscrito al topico %s",g_topic_handlers[i].topic);
            }
         }
  }