import paho.mqtt.client as mqtt
import sqlite3
from datetime import datetime
from PIL import Image
import io
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
       
        if msg.topic == "abrir_puerta/identificador": 
            message = msg.payload.decode()
            self.identificador = int(message)
            print(f"Mensaje recibido en {msg.topic}: {self.identificador}")
        
        elif msg.topic == "abrir_puerta/camara":
            print(f"Mensaje recibido en {msg.topic}")
            if(self.identificador is not None):
                print("Identificador is not none realizar consulta")
                self.crsr.execute("SELECT * FROM Personas_con_acceso WHERE identificador = ?", (self.identificador,))
                resultado = self.crsr.fetchone()
                print(f"Resultado consulta {resultado}")
                accept = None
                if(resultado is not None):
                    accept = "YES"
                else:
                    accept = "NO"
                print(f"Insertar todos los datos en la tabla para ello aceptar paso : {accept}, idenficador {self.identificador}")
                self.crsr.execute('INSERT INTO entradas VALUES (?,?,?,?)',
                       (datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                        self.identificador,
                        accept,
                        msg.payload
                        )
                )
                self.conn_bd.commit()
                #además enviamos el mensaje al topic para que se active el servomotor
                self.client_mqtt.publish("abrir_puerta/Permitir_paso",accept,qos = 2)
                self.identificador = None # PARA que no coja el valor anterior
            
            
    def run(self):
        # Mantener la conexión para recibir mensajes
        self.client_mqtt.loop_forever()

    def stop(self):
        self.client_mqtt.loop_stop()

if __name__ == "__main__":
    
    bridge = BaseDataMQTTBridge(base_data="Control_Puerta.db",mqtt_port=1883)
    # Configurar el manejador de señal
    try:
        bridge.connect_to_mqtt("abrir_puerta/identificador","abrir_puerta/camara")
        bridge.run()
    finally:
        bridge.stop()
           
    
