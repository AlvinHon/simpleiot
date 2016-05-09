## All Copyrights reserved. alvin.cpp@gmail.com , 2016
##
## This program acts as mqtt client to receives messages from a remote IoT device through MQTT 
## http://mqtt.org/ .
## Regardless of what messages of the IoT Device convey, the program just publish to redis server
## so that the redis client can act accordingly.
##
import paho.mqtt.client as mqtt
import time
import redis
import json

with open('.config') as df:
	CONFIG = json.load(df)

MQTT_CONNECT_HOST = CONFIG["MQTT_CONNECT_HOST"]
MQTT_CONNECT_PORT = CONFIG["MQTT_CONNECT_PORT"]
MQTT_TOPIC = CONFIG["MQTT_TOPIC"]
RQUEUE_NUMADD=CONFIG["RQUEUE_NUMADD"]

r = redis.StrictRedis()

def on_connect(client, data, flags, rc):
	print("connected "+str(rc))
	client.subscribe("atopic") 


def on_message(client, data, msg):
	if str(msg.topic) == MQTT_TOPIC:
		print(msg.topic+" "+str(msg.payload))
		r.publish(RQUEUE_NUMADD,'')


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_CONNECT_HOST,MQTT_CONNECT_PORT,60)

client.loop_forever()

