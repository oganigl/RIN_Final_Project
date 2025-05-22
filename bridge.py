import paho.mqtt.client as mqtt
import sqlite3
from datetime import datetime
class BaseDataMQTTBridge:
    def __init__(self,base_data,mqtt_port):
        #mqtt variables para conectarse como cliente de mqtt
        self.conn_bd = sqlite3.connect(base_data)
        self.client_mqtt = mqtt.Client()
        self.crsr = self.conn_bd.cursor()
        self.mqtt_port = mqtt_port
        self.identification = None
        self.image = None
        
    def connect_to_mqtt(self,*topics):
        self.client_mqtt.connect("localhost", self.mqtt_port, 60)
        print("Conectado al broker")
        # Suscribirse al tema
        for topic in topics:
            self.client_mqtt.subscribe(topic)
            
        # Asignar la función de callback
        self.client_mqtt.on_message = self.on_message
     
     # Función de callback que se ejecuta cuando se recibe un mensaje   
    def on_message(self,client, userdata, msg):
        message = msg.payload.decode()
        if msg.topic == "abrir_puerta/identificador":
            self.identificador = int(message)
            print(f"Mensaje recibido en {msg.topic}: ")
        
        elif msg.topic == "abrir_puerta/camara":
            #una vez recibimos la imagen añadimos todo a   sql
            if(self.identificador is not None):
                self.crsr.execute("SELECT * FROM Personas_con_acceso where identificador= {self.identificador}")
                resultado = self.crsr.fetchone()
                accept = None
                if(resultado is None):
                    accept = "YES"
                else:
                    accept = "NO"
                self.crsr.execute('INSERT INTO entradas VALUES (?,?,?,?)',
                       (self.identificador,
                        message,
                        datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                        accept
                        )
                )
           
            print(f"Mensaje recibido en {msg.topic}")
            #además enviamos el mensaje al topic para que se active el servomotor
            self.client_mqtt.publish("abrir_puerta/Permitir_paso",accept,qos = 2)
    def run(self):
        # Mantener la conexión para recibir mensajes
        self.client_mqtt.loop_forever()


if __name__ == "__main__":
    
    bridge = BaseDataMQTTBridge(base_data="Control_Puerta.db",mqtt_port=1883)
    # Configurar el manejador de señal
    try:
        bridge.connect_to_mqtt("abrir_puerta/identificador","abrir_puerta/camara")
        bridge.run()
    finally:
        bridge.close_server()
           
    
