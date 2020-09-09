import paho.mqtt.client as MQTT
import requests
import string
import json
import time

url="http://localhost:8080"

class subscriber:
		def __init__(self, clientID,topic,broker):
			self.clientID = clientID
			# create an instance of paho.mqtt.client
			self._paho_mqtt = MQTT.Client(clientID, False) 

			# register the callback
			self._paho_mqtt.on_connect = self.myOnConnect
			self._paho_mqtt.on_message = self.myOnMessageReceived

			self.topic = topic
			self.messageBroker =broker 

		def start (self):
			#manage connection to broker
			self._paho_mqtt.connect(self.messageBroker, 1883)
			self._paho_mqtt.loop_start()
			# subscribe for a topic
			self._paho_mqtt.subscribe(self.topic, 2)

		def stop (self):
			self._paho_mqtt.unsubscribe(self.topic)
			self._paho_mqtt.loop_stop()
			self._paho_mqtt.disconnect()
		def myOnConnect (self, paho_mqtt, userdata, flags, rc):
			print ("Connected to %s with result code: %d" % (self.messageBroker, rc))
		def myOnMessageReceived (self, paho_mqtt , userdata, msg):
			json_body = json.loads(msg.payload.decode("utf-8"))
			print (f"Si registra una temperatura di {json_body['e'][0]['v']} gradi {json_body['e'][0]['u']}")




if __name__=='__main__':
	while True:

		data={'ID':'serv1','Description':'Lettura temperatura','EndPoints':None}
		rpost=requests.post(url+'/services',json.dumps(data))
		rget=requests.get(url+'/infoMQTT')
		json_info=rget.json()
		

		rgetTopic=requests.get(url+'/devices?ID=ArduinoTemp')
		topica=rgetTopic.json()[0]['EndPoints']
		print(topica)

		
		sub=subscriber('s','iot/21/'+topica,json_info.get('Broker'))
		
		sub.start()
		time.sleep(60)
		sub.stop()
