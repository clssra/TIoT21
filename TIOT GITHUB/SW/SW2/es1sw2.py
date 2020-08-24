import cherrypy
import json
import string
import threading
import time

# database interno, no gestione con file
generalInfo = {'IP': '127.0.0.1', 'Port': '8080'}
users = {}
devices = {}
services = {}


class checkTime(threading.Thread):
    def __init__(self, tid):
        threading.Thread.__init__(self)

    def run(self):
        # leggi i files e cancella se time-timestamp è più di due minuti

        for i in devices.keys():
            if (time.time() - devices.get(i).get('TimeStamp')) > 120:
            	devices.pop(i)
        for i in services.keys():
            if (time.time() - services.get(i).get('TimeStamp')) > 120:
                services.pop(i)


class Catalog():

    exposed = True

    # GET è usato per le ricerche nei dizionari
    def GET(self, *uri, **params):
        if len(params) > 1:
            return "wrong number of params"
        if(len(uri)>0):
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
                    return json.dumps(service, indent=4)
                else:
                    return json.dumps(services.get(params.get('ID')),indent=4)
            elif uri[0] == 'info':
                return json.dumps(generalInfo)
            else:
                return "404 error: page not found"

    # POST è usato per l'inserimento di nuovi valori nei database
    def POST(self, *uri, **params):
        if uri[0] == 'devices':
            if len(params) == 3:
                timestamp = time.time()
                devices.update({params.get('ID'): {'EndPoints': params.get(
                    'endpoint'), 'AvailResurces': params.get('resurces'), 'TimeStamp': timestamp}})
            else:
                return "wrong number of params"
        elif uri[0] == 'users':
            if len(params) == 4:
                users.update({params.get('ID'): {'Name': params.get(
                    'name'), 'Surname': params.get('surname'), 'Email Address': params.get('email')}})
            else:
                return "wrong number of params"
        elif uri[0] == 'services':
            if len(params) == 3:
                timestamp = time.time()
                services.update({params.get('ID'): {'Description': params.get(
                    'description'), 'EndPoints': params.get('endpoint'), 'TimeStamp': timestamp}})
            else:
                return "wrong number of params"
        else:
            return "404 error: page not found"

    '''	body=cherrypy.request.body.read()
		json_body=json.loads(body.decode('utf-8'))
		if uri[0]=='devices':
			devices.update(json_body)
		elif uri[0]=='users':
			users.update(json_body)
		elif uri[0]=='services':
			services.update(json_body)'''


if __name__ == '__main__':

    i = 0
    conf = {
        '/': {
            'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
            'tools.sessions.on': True
        }
    }
    cherrypy.tree.mount(Catalog(), '/', conf)
    cherrypy.engine.start()
    while True:
            time.sleep(60)
            i += 1
            check = checkTime(i)
            check.start()
            check.join()
    cherrypy.engine.block()

