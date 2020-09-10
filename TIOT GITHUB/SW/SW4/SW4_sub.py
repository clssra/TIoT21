import paho.mqtt.client as mqtt
import json
import time
import requests
import string
from onAllarm import mailFile

class subscriber():
	def __init__(self, cid, broker, port, topic):
		self.cid=cid
		self.broker=broker
		self.port=port
		self.topic=topic
		self.csmqtt=mqtt.Client(cid, False)
		self.csmqtt.on_connect=self.myOnConnect
		self.csmqtt.on_message=self.myOnMessage

	def start(self):
		self.csmqtt.connect(self.broker, self.port)
		self.csmqtt.loop_start()
		self.csmqtt.subscribe(self.topic,2)

	def stop(self):
		self.csmqtt.unsubscribe(self.topic)
		self.csmqtt.loop_stop()
		self.csmqtt.disconnect()

	

	def myOnConnect (self, csmqtt, userdata, flags, rc):
		print ("Connected to %s topic %s with result code: %d" % (self.messageBroker, self.topic, rc))
	def myOnMessage (self, csmqtt , userdata, msg):
		print ("Topic:'" + msg.topic+"', QoS: '"+str(msg.qos)+"' Message: '"+str(json.loads(msg.payload)) + "'")
		print(((json.loads(msg.payload)).get("e"))[0].get("v"))
		print(msg.topic)

		userslist=requests.get('http://localhost:8080/users').json()
		for user in userslist.values():
			mailFile(user.get('Name'),user.get('Surname'),user.get('Email Address'), time.ctime(time.time()))

			

if __name__ == '__main__':
		sub=[]
		data={'ID':'serv1','Description':'Gestore allarme','EndPoints':'iot/21/alarm'}
		rpost=requests.post('http://localhost:8080/services',json.dumps(data))
		rget=requests.get('http://localhost:8080/infoMQTT')
		json_info=rget.json()

		rgetTopic=requests.get(url+"/devices?ID=Arduino")
		topica=rgetTopic.json()[0]['EndPoints']

		sub=subscriber('s1', json_info.get('Broker'), int(json_info.get('Port')), 'iot/21'+topica)
		sub.start()


		time.sleep(4)

		time.sleep(100)
		sub.stop()
