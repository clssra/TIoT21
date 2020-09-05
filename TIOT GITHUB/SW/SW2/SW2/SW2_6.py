import requests
import json
import random
import time
import paho.mqtt.client as MQTT
import time

class publisher:
	def __init__(self, clientID):
		self.clientID = clientID
		self.broker = 'test.mosquitto.org'
		self.mqttclient = MQTT.Client(self.clientID, False) 
		self.mqttclient.on_connect = self.myConnect

	def start (self):
		self.mqttclient.connect(self.broker, 1883)
		self.mqttclient.loop_start()

	def stop (self):
		self.mqttclient.loop_stop()
		self.mqttclient.disconnect()

	def myPublish(self, topic, message):
		self.mqttclient.publish(topic, message, 2)

	def myConnect (self, paho_mqtt, userdata, flags, rc):
		print ("Connected to %s with result code: %d" % (self.broker, rc))

devices={}

if __name__=='__main__':
	mqttc = publisher("IOT_SERVICES")
	mqttc.start()
	while True:
		
		ID=random.randint(0,10)
		endpoints='prova_v'+str(ID)
		resurces='astrolabo gravitazionale mk'+str(ID)
		timestamp=time.time()
		devices.update({ID:{'EndPoints':endpoints,'AvailResurces':resurces,"TimeStamp":timestamp}})
		data={'bn':ID, 'e':[{'n':'lightning' ,'u':'amph','t':timestamp,'v':ID*10},{'n':'lightning' ,'u':'amph','t':timestamp,'v':ID*20}]}
		mqttc.myPublish('iot/21/devices',json.dumps(data))
		time.sleep(6)
	mqttc.stop()
