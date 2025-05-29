import paho.mqtt.client as mqtt
from PIL import Image
import io

def on_message(client, userdata, msg):
    print(f"Mensaje recibido en {msg.topic}, tamaño: {len(msg.payload)} bytes")

    # Guardar el payload sin intentar abrirlo aún
    with open("imagen_recibida.jpeg", "wb") as f:
        f.write(msg.payload)
    print("Imagen guardada como imagen_recibida.jpeg")

    # Intentar abrir la imagen
    try:
        image = Image.open(io.BytesIO(msg.payload))
        image.show()
    except Exception as e:
        print("Error al procesar la imagen:", e)

client = mqtt.Client()
client.on_message = on_message
client.connect("localhost", 1883)
client.subscribe("abrir_puerta/camara", qos=1)
client.loop_forever()
