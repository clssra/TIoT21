import cherrypy
import json
import string
import threading
import time

# database interno, no gestione con file
generalInfo = {'IP': '127.0.0.1', 'Port': '8080'}
generalInfoMQTT = {'Broker': 'test.mosquitto.org', 'Port': '1883'}

devicelist={}
servicelist={}
userlist={}

class checkTime(threading.Thread):
    def __init__(self, tid):
        threading.Thread.__init__(self)

    def run(self):
        # leggi i files e cancella se time-timestamp è più di due minuti
        services=open("services.txt","r")
        devices=open("devices.txt","r")

        try:
            for i in list(json.load(devices).keys()):
                if (time.time()- devicelist[i][0]['TimeStamp']) > 120:
                    devicelist.pop(i)
        except:
            pass
        try:
            for i in list(json.load(services).keys()):
                if (time.time() - servicelist[i]['TimeStamp']) > 120:
                    servicelist.pop(i)
        except: 
            pass
        services.close()
        devices.close()

        services=open("services.txt","w")
        devices=open("devices.txt","w")
        json.dump(devicelist,devices)
        json.dump(servicelist,services)
        services.close()
        devices.close()

#SenML {p, e:[]}

class catalog():

    exposed = True

    # GET è usato per le ricerche nei dizionari
    def GET(self, *uri, **params):
        if len(params) > 1:
            return "wrong number of params"
        if len(uri)<1:
        	return 'Errore'
        if uri[0] == 'devices':
            devices=open("devices.txt","r")
            if bool(params) == False:
                dev=json.dumps(json.load(devices),indent=4)
                devices.close()
                return dev
            else:
                dev=json.dumps((json.load(devices)).get(params.get("ID")),indent=4)
                devices.close()
                return dev
        elif uri[0] == 'users':
            users=open("users.txt","r")
            if bool(params) == False:
                us=json.dumps(json.load(users),indent=4)
                users.close()
                return us
            else:
                us=json.dumps((json.load(users)).get(params.get("ID")),indent=4)
                users.close()
                return us
        elif uri[0] == 'services':
            services=open("services.txt","r")
            services=open("services.txt","r")
            if bool(params) == False:
                serv=json.dumps(json.load(services),indent=4)
                services.close()
                return serv
            else:
                serv=json.dumps((json.load(services)).get(params.get("ID")),indent=4)
                services.close()
                return serv
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
            if len(json_body) != 1:
                timestamp = time.time()
                lun=len(json_body['e'])
                devicelist.update({json_body.get('bn'):[]})
                for i in range(lun):
                    devicelist[json_body.get('bn')].append({'EndPoints': (json_body.get('e')[i]).get('n'), 'AvailResurces': (json_body.get('e')[i]).get('n'), 'TimeStamp': timestamp})
                devices=open("devices.txt","w")
                json.dump(devicelist,devices, indent=4)
                devices.close()
            else:
                return "wrong number of params"
    	elif uri[0] == 'users':
            if len(json_body) == 4:
                userlist[json_body.get('ID')]={'Name': json_body.get('name'), 'Surname': json_body.get('surname'), 'Email Address': json_body.get('email')}
                users=open("users.txt","w")
                json.dump(userlist,users, indent=4)
                users.close()
            else:
                return "wrong number of params"
    	elif uri[0] == 'services':
            if len(json_body) == 3:
                timestamp = time.time()
                servicelist[json_body.get('ID')]={'Description': json_body.get('Description'), 'EndPoints': json_body.get('EndPoints'), 'TimeStamp': timestamp}
                services=open("services.txt","w")
                json.dump(servicelist,services, indent=4)
                services.close()
            else:
                return "wrong number of params"
    	else:
            return "404 error: page not found"



if __name__ == '__main__':
    try:
        users=open("users.txt","r")
        if(users.read()!=""):
            users.seek(0)
            userlist=json.load(users)
    except:
        users=open("users.txt","x")
    users.close()

    try:
        services=open("services.txt","r")
        if(services.read()!=""):
            services.seek(0)
            servicelist=json.load(services)
    except:
        services=open("services.txt","x")
    services.close()

    try:
        devices=open("devices.txt","r")
        if(devices.read()!=""):
            devices.seek(0)
            devicelist=json.load(devices)
    except:
        devices=open("devices.txt","x")
    devices.close()

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

    while True:
            time.sleep(10)
            i += 1
            check = checkTime(i)
            check.start()
            check.join()

    cherrypy.engine.block()


    


