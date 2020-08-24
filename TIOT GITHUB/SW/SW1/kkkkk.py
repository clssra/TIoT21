import json
import cherrypy

class Arduino():
	exposed = True;
	#def GET(self, *uri, **params):
	def __init__(self):
		self.kk = []

	def POST(self, **params):
		body = cherrypy.request.body.read()
		json_body = json.loads(body.decode('utf-8'))
		self.kk.append(json_body)
		return json.dumps(json_body, indent = 4)

	def GET(self, **params):
		return json.dumps(self.kk, indent = 4)


if __name__ == '__main__':
    
    conf={
            '/':{
                'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
                'tool.session.on': True
                }
    }
    cherrypy.tree.mount(Arduino(), '/log', conf)
    cherrypy.config.update({'server.socket_host': '0.0.0.0'})
    cherrypy.config.update({'server.socket_port': 8080})
    cherrypy.engine.start()
    cherrypy.engine.block()
