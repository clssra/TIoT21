import cherrypy
import json
import string
import threading
import time
import paho.mqtt.client as PahoMQTT

# database interno, no gestione con file
generalInfo = {'IP': '127.0.0.1', 'Port': '8080'}
generalInfoMQTT = {'Broker': 'test.mosquitto.org', 'Port': '1883'}
users = {}
devices = {}
services = {}
tempdev={}
tempser={}


class checkTime(threading.Thread):
    def __init__(self, tid):
        threading.Thread.__init__(self)

    def run(self):
        # leggi i files e cancella se time-timestamp è più di due minuti

        for i in devices.keys():
            if (time.time() - devices.get(i)[0].get('TimeStamp')) < 120:
            	tempdev[i]=devices[i]
        for i in services.keys():
            if (time.time() - services.get(i).get('TimeStamp')) < 120:
                tempser[i]=services[i]

        devices.clear()
        services.clear()

        devices.update(tempdev)
        services.update(tempser)

        tempdev.clear()
        services.clear()


class catalog():

    exposed = True

    # GET è usato per le ricerche nei dizionari
    def GET(self, *uri, **params):
        if len(params) > 1:
            return "wrong number of params"
        if len(uri)<1:
        	return 'Errore'
        if uri[0] == 'devices':
            if bool(params) == False:
                return json.dumps(devices,indent=4)
            else:
                return json.dumps(devices.get(params.get('ID')),indent=4)
        elif uri[0] == 'users':
            if bool(params) == False:
                return json.dumps(users,indent=4)
            else:
                return json.dumps(users.get(params.get('ID')),indent=4)
        elif uri[0] == 'services':
            if bool(params) == False:
                return json.dumps(services, indent=4)
            else:
                return json.dumps(services.get(params.get('ID')),indent=4)
        elif uri[0] == 'info':
            return json.dumps(generalInfo)
        elif uri[0] == 'infoMQTT':
            return json.dumps(generalInfoMQTT)
        else:
            return "404 error: page not found"

    # POST è usato per l'inserimento di nuovi valori nei database
    def POST(self, *uri):
    	body=cherrypy.request.body.read()
    	json_body=json.loads(body.decode('utf-8'))
    	if uri[0] == 'devices':
            if len(json_body) == 2:
                timestamp = time.time()
                devices.update({json_body.get('bn'):[]})
                lun=len(json_body['e'])
                for i in range(lun):
	                devices[json_body.get('bn')].append({'EndPoints': (json_body.get(
	                    'e')[i]).get('n'), 'AvailResurces': (json_body.get('e')[i]).get('n'), 'TimeStamp': timestamp})
            else:
                return "wrong number of params"
    	elif uri[0] == 'users':
            if len(json_body) == 4:
                users.update({json_body.get('ID'): {'Name': json_body.get(
                    'name'), 'Surname': json_body.get('surname'), 'Email Address': json_body.get('email')}})
            else:
                return "wrong number of params"
    	elif uri[0] == 'services':
            if len(json_body) == 3:
                timestamp = time.time()
                services.update({json_body.get('ID'): {'Description': json_body.get(
                    'Description'), 'EndPoints': json_body.get('EndPoints'), 'TimeStamp': timestamp}})
            else:
                return "wrong number of params"
    	else:
            return "404 error: page not found"

class MySubscriber:
    def __init__(self, clientID,topic,broker):
        self.clientID = clientID
        self._paho_mqtt = PahoMQTT.Client(clientID, False)

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
        json_body = json.loads(msg.payload)
        #json_body=json.loads(body.decode('utf-8'))
        if len(json_body) == 2:
            timestamp = time.time()
            devices.update({json_body.get('bn'):[]})
            lun=len(json_body['e'])
            for i in range(lun):
                devices[json_body.get('bn')].append({'EndPoints': (json_body.get('e')[i]).get('n'), 'AvailResurces': (json_body.get('e')[i]).get('n'), 'TimeStamp': timestamp})
        else:
            return "wrong number of params"

    

if __name__ == '__main__':

    i = 0
    conf = {
        '/': {
            'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
            'tools.sessions.on': True
        }
    }
    cherrypy.config.update({'server.socket_host':'0.0.0.0'})
    cherrypy.config.update({'server.socket_port':8080})
    cherrypy.tree.mount(catalog(), '/', conf)

    cherrypy.engine.start()

    mqttC = MySubscriber("Subscriber 1","iot/21/devices",'test.mosquitto.org')
    mqttC.start()
    while True:
            time.sleep(10)
            i += 1
            check = checkTime(i)
            check.start()
            check.join()
    cherrypy.engine.block()
    mqttC.stop()






