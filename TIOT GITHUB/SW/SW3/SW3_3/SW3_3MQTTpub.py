import paho.mqtt.client as MQTT
import requests
import string
import json
import time

url="http://localhost:8080"

class publisher():
	def __init__(self, cid, broker, port):
		self.cid=cid
		self.cmqtt=MQTT.Client(self.cid, False)
		self.cmqtt.on_connect=self.MyOnConnect
		
		self.broker=broker
		self.port=port

	def start(self):
		self.cmqtt.connect(self.broker, self.port)
		self.cmqtt.loop_start()
		

	def stop(self):
		self.cmqtt.loop_stop()
		self.cmqtt.disconnect()

	def MyPublish (self, topic, message):
		self.cmqtt.publish(topic, message, 2)

	def MyOnConnect(self, cmqtt, userdata, flags, rc):
		print('connected to %s with result code %d'%(self.broker, rc))

if __name__=='__main__':

	data={'ID':'serv2','Description':'Scrittura led','EndPoints':None}
	requests.post(url+'/services', data)
	rget=requests.get(url+'/infoMQTT')
	
	json_info=rget.json()
	print(json_info.get('Port'))

	pub=publisher('pub1',json_info.get('Broker'), int(json_info.get('Port')))
	pub.start()

	rgetTopic=requests.get(url+"/devices?ID=ArduinoLed")
	topica=rgetTopic.json()[0]['EndPoints']

	while True:

		led=input('Accesa (1) o spernta (0)')
	
		timestamp=time.time()
		
		pub.MyPublish('iot/21/'+topica, json.dumps({'bn':'ArduinoLed','e':[{'n':'led','u':None,'t':timestamp,'v':int(led)}]}))
		time.sleep(6)

	pub.stop()


