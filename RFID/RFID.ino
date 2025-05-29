// // src/main.cpp

// #include <SPI.h>
// #include <MFRC522.h>

// #define RST_PIN     9
// #define SS_PIN      10
// #define SCK_PIN     12
// #define MISO_PIN    13
// #define MOSI_PIN    11

// SPIClass spiRFID(HSPI);
// MFRC522 mfrc522(SS_PIN, RST_PIN);

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // Inicializar SPI en HSPI
//   spiRFID.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);

//   // 🔁 Redirigir SPI global (usado por la librería) a nuestro HSPI
//   SPI = spiRFID;

//   // Inicializar MFRC522 usando SPI redirigido
//   mfrc522.PCD_Init();

//   Serial.println("Acerque una tarjeta RFID al lector...");
// }

// void loop() {
//   if (!mfrc522.PICC_IsNewCardPresent()) return;
//   if (!mfrc522.PICC_ReadCardSerial()) return;

//   Serial.print("UID de la tarjeta: ");
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
//     Serial.print(" ");
//   }
//   Serial.println();
// }
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

// Pines RFID (usando HSPI)
#define RST_PIN     9
#define SS_PIN      10
#define SCK_PIN     12
#define MISO_PIN    13
#define MOSI_PIN    11

// Servo
#define SERVO_PIN 4 

// WiFi
const char* ssid = "SAMSUNG";
const char* password = "12345678";

// MQTT
const char* mqtt_server = "192.168.7.24";  // sin "mqtt://"
const char* topic_rfid = "abrir_puerta/identificador";
const char* topic_servo = "abrir_puerta/Permitir_paso";

SPIClass spiRFID(HSPI);
MFRC522 mfrc522(SS_PIN, RST_PIN);
WiFiClient espClient;
PubSubClient client(espClient);
Servo miServo;  // ✅ Objeto servo

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializar SPI en HSPI
  spiRFID.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  SPI = spiRFID;

  // Inicializar RFID
  mfrc522.PCD_Init();
  Serial.println("Acerque una tarjeta RFID al lector...");

  // Conexión WiFi y MQTT
  conectarWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callbackMQTT);

  // Inicializar servo
  miServo.attach(SERVO_PIN);
  miServo.write(90);  // posición inicial
}

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();

  // Leer RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }

    Serial.print("Tarjeta leída: ");
    Serial.println(uid);

    if (compararUID(mfrc522.uid.uidByte, mfrc522.uid.size, (byte[]){0x73, 0x72, 0xE3, 0x0D}, 4)) {
      client.publish(topic_rfid, "2");
    } else if (compararUID(mfrc522.uid.uidByte, mfrc522.uid.size, (byte[]){0xF3, 0x07, 0xD7, 0x11}, 4)) {
      client.publish(topic_rfid, "3");
    } else {
      client.publish(topic_rfid, uid.c_str());
    }

    delay(1000); // evitar doble lectura
    mfrc522.PICC_HaltA();
  }
}

// Manejo de mensajes MQTT entrantes
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (unsigned int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  Serial.print("Mensaje MQTT recibido en [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensaje);

  if (String(topic) == topic_servo) {
    if (mensaje == "YES") {
      Serial.println("🟢 Abriendo puerta...");
      miServo.write(0);  // abrir
      delay(2000);
      miServo.write(90); // cerrar
    }
  }
}

void conectarWiFi() {
  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conectado.");
}

void reconectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32_RFID_Client")) {
      Serial.println(" conectado al broker.");
      client.subscribe(topic_servo);
    } else {
      Serial.print(" falló, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

bool compararUID(byte* uid, byte uidSize, byte* referencia, byte refSize) {
  if (uidSize != refSize) return false;
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != referencia[i]) return false;
  }
  return true;
}
