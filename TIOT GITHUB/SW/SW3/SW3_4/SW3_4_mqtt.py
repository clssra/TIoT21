import paho.mqtt.client as MQTT
import requests
import string
import json
import time

url="http://localhost:8080"

temperatura=0



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
		print("sto inviando")

	def MyOnConnect(self, cmqtt, userdata, flags, rc):
		print('connected to %s with result code %d'%(self.broker, rc))




class MySubscriber:
	def __init__(self, clientID,topic,broker,port):
		self.clientID = clientID
		self._paho_mqtt = MQTT.Client(clientID, False)

		self._paho_mqtt.on_connect = self.myOnConnect
		self._paho_mqtt.on_message = self.myOnMessageReceived

		self.topic = topic
		self.messageBroker =broker
		self.port=port

	def start (self):
		self._paho_mqtt.connect(self.messageBroker, self.port)
		self._paho_mqtt.loop_start()

		self._paho_mqtt.subscribe(self.topic, 2)

	def stop (self):
		self._paho_mqtt.unsubscribe(self.topic)
		self._paho_mqtt.loop_stop()
		self._paho_mqtt.disconnect()

	def myOnConnect (self, paho_mqtt, userdata, flags, rc):
		print ("Connected to %s with result code: %d" % (self.messageBroker, rc))

	def myOnMessageReceived (self, paho_mqtt , userdata, msg):
		print(str(msg.payload.decode("utf-8")))
		json_body = json.loads(msg.payload.decode("utf-8"))
		if(json_body['e'][0]['n']=="temperature"):
			global temperatura
			temperatura=float(json_body['e'][0]['v'])
			print (f"Si registra una temperatura di {temperatura} gradi {json_body['e'][0]['u']}")
		elif(json_body['e'][0]['n']=="noise"):
			if(json_body['e'][0]['v']==1):
				print (f"Si registra un rumore")
			global noise
			global noisetime
			noisetime=time.time()
			noise=True
		elif(json_body['e'][0]['n']=="movement"):
			if(json_body['e'][0]['v']==1):
				print (f"Si registra un movimento")
			global movement
			global movementtime
			movementtime=time.time()
			movement=True

def publishconditioning(pubcond, topic, ACMax, ACmin, HTMax, HTmin):
	global temperatura	
	print(temperatura)

	if temperatura>ACmin and temperatura<ACMax:
		fan=(temperatura-ACmin)*255/(ACMax-ACmin)
	elif temperatura<=ACmin:
		fan=0
	else:
		fan=255

	if temperatura>HTmin and temperatura<HTMax:
		led=((HTMax-HTmin)-(temperatura-HTmin))*255/(HTMax-HTmin)
	elif temperatura<=HTmin:
		led=255
	else:
		led=0
	print(str(fan)+', fan')
	pubcond.MyPublish(topic, json.dumps({'bn':'Arduino','e':[{'n':'fan','u':"Percentage",'t':time.time(),'v':fan}]}))
	print(str(led)+', led')
	pubcond.MyPublish(topic, json.dumps({'bn':'Arduino','e':[{'n':'led','u':"Percentage",'t':time.time(),'v':led}]}))


movement=False
noise=False
movementtime=time.time()
noisetime=time.time()

if __name__=='__main__':
	


	data={'ID':'serv2','Description':'Scrittura led','EndPoints':None}
	requests.post(url+'/services', data)
	rget=requests.get(url+'/infoMQTT')
	
	json_info=rget.json()

	rgetTopic=requests.get(url+"/devices?ID=Arduino")
	topica=rgetTopic.json()[0]['EndPoints']
	topicb=rgetTopic.json()[1]['EndPoints']
	ACMax=[35, 30]
	ACmin=[25, 22]
	HTMax=[20, 18]
	HTmin=[10, 7]

	sub=MySubscriber('sub','iot/21/'+topica,json_info.get('Broker'), int(json_info.get('Port')))
	sub.start()
	pubcond=publisher('pubcond',json_info.get('Broker'), int(json_info.get('Port')))
	pubcond.start()
	pubDisplay=publisher('pubDisplay',json_info.get('Broker'), int(json_info.get('Port')))
	pubDisplay.start()

	time.sleep(10)

	while True:
		scelta=input("Inserisci 'c' per valori di temperatura personalizzati,\n'q' per chiudere\n'd' per scrivere sul display\nqualunque altro carattere per i valori di default")
		
		if scelta=='c':
			ACMaxCust=int(input("ACMax:"))
			ACminCust=int(input("ACmin:"))
			HTMaxCust=int(input("HTMax:"))
			HTminCust=int(input("HTmin:"))
			publishconditioning(pubcond, 'iot/21/'+topicb, ACMaxCust, ACminCust, HTMaxCust, HTminCust)
			
		elif scelta=='q':
			break	

		elif scelta=='d':
			
			
			testo=input("Cosa vuoi scrivere?")
			pubDisplay.MyPublish('iot/21/'+topicb, json.dumps({'bn':'Arduino','e':[{'n':'display','u':"Percentage",'t':0,'v':testo}]}))
			
		
		else:
			a=0
			if noise or movement:
				a=1
			publishconditioning(pubcond, 'iot/21/'+topicb, ACMax[a], ACmin[a], HTMax[a], HTmin[a])

		
		time.sleep(5)

		if time.time()-movementtime>120:
			#global movement
			movement=False

		if time.time()-noisetime>120:
			#global noise
			noise=False
	pubDisplay.stop()
	sub.stop()
	pubcond.stop()

