import paho.mqtt.client as MQTT
import time
import json


class subscriber:
		def __init__(self, clientID,topic,broker):
			self.clientID = clientID
			self._paho_mqtt = MQTT.Client(clientID, False) 
			self._paho_mqtt.on_connect = self.myOnConnect
			self._paho_mqtt.on_message = self.myOnMessageReceived
			self.topic = topic
			self.messageBroker =broker 

		def start (self):

			self._paho_mqtt.connect(self.messageBroker, 1883)
			self._paho_mqtt.loop_start()

			self._paho_mqtt.subscribe(self.topic, 2)

		def stop (self):
			self._paho_mqtt.unsubscribe(self.topic)
			self._paho_mqtt.loop_stop()
			self._paho_mqtt.disconnect()

		def myOnConnect (self, paho_mqtt, userdata, flags, rc):
			print ("Connected to %s with result code: %d" % (self.messageBroker, rc))

		def myOnMessageReceived (self, paho_mqtt , userdata, msg):
			print ("Topic:'" + msg.topic+"', QoS: '"+str(msg.qos)+"' Message: '"+str(msg.payload) + "'")



if __name__ == "__main__":
	subsc1=subscriber('s1','iot/21/+', 'test.mosquitto.org')
	subsc2=subscriber('s2', 'iot/#', 'test.mosquitto.org')
	subsc3=subscriber('s3', 'iot/+/prova', 'test.mosquitto.org')
	subsc1.start()
	subsc2.start()
	subsc3.start()
	time.sleep(60)
	subsc1.stop()
	subsc2.stop()
	subsc3.stop()
