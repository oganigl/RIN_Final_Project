#include "mqtt.h"
const char* MQTT_TAG = "MQTT";
static const mqtt_topic_handler_t *g_topic_handlers = NULL;
static int g_num_topic_handlers = 0;
static esp_mqtt_client_handle_t client;
static void mqtt_event_handler(esp_mqtt_event_handle_t event){
    if(event->event_id == MQTT_EVENT_CONNECTED){
        ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client,"your topic",0); //in mqtt we require a topic to subscribe and client is from event client and 0 is quality of service it can be 1 or 2
        ESP_LOGI(MQTT_TAG, "sent subscribe successful" );
        }
     else if(event->event_id == MQTT_EVENT_DISCONNECTED)
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED"); 
      }
      else if(event->event_id == MQTT_EVENT_SUBSCRIBED)
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED");
      }
      else if(event->event_id == MQTT_EVENT_UNSUBSCRIBED) 
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED");
      }
      else if(event->event_id == MQTT_EVENT_DATA){
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
      else if(event->event_id == MQTT_EVENT_ERROR)//when any error
      {
         ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR");
      }
}

void mqtt_start(char* username,char* password,const mqtt_topic_handler_t topic_handlers[],int num_handlers)
{/*Depending on your website or cloud there could be more parameters in mqtt_cfg.*/
  esp_mqtt_client_config_t mqtt_cfg={
    .broker.address.uri="mqtt://io.adafruit.com", //Uniform Resource Identifier includes path,protocol
    .credentials.username= username/*your username*/, //your username
    .credentials.authentication.password = password/*your adafruit password*/, //your adafruit io password
  };
  
  client = esp_mqtt_client_init(&mqtt_cfg); //sending struct as a parameter in init client function
  esp_mqtt_client_register_event(client,ESP_EVENT_ANY_ID,mqtt_event_handler,client);
  esp_mqtt_client_start(client); //starting the process
  g_topic_handlers = topic_handlers;
  g_num_topic_handlers = num_handlers;
   for(int i = 0; i < num_handlers;i++){
    if(esp_mqtt_client_subscribe(client, topic_handlers[i].topic, 2) != ESP_OK) //QOs == 2 Debido a que queremos asegurarnos que los mensajes lleguen exactamente una vez
    {
       ESP_LOGW(MQTT_TAG,"Error al subscribirse al topico %s",topic_handlers[i].topic);
    }
   }
  }
  int send_message(const char* topic,char* data,int len){
   return esp_mqtt_client_publish(client,topic,data,len,2,false);
   
  }