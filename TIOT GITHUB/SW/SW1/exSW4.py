import cherrypy
import json
import os, os.path

class Dashboard(object):

	exposed=True
	def GET(self, *uri):
		return open("index.html")

	def POST(self, *uri, **params):
		if uri[0]=="saveDashboard":
			a=open("dashboard/dashboard.json","w")

			a.write(params['json_string'])

if __name__ == '__main__':
	conf = {
	'/': {
	'request.dispatch':cherrypy.dispatch.MethodDispatcher(),
	'tools.sessions.on': True,
	'tools.staticdir.root': os.path.abspath(os.getcwd())
	},
	"/css": {
	'tools.staticdir.on': True,
	'tools.staticdir.dir':'css'
	},
	"/js": {
	'tools.staticdir.on': True,
	'tools.staticdir.dir':'js'
	},
	"/img": {
	'tools.staticdir.on': True,
	'tools.staticdir.dir':'img'
	},
	"/plugin": {
	'tools.staticdir.on': True,
	'tools.staticdir.dir':'plugins'
	},
	"/dashboard": {
	'tools.staticdir.on': True,
	'tools.staticdir.dir':'dashboard'
	}
	}
	cherrypy.tree.mount (Dashboard(), '/', conf)
	cherrypy.config.update({'server.socket_host': '0.0.0.0'})
	cherrypy.config.update({'server.socket_port': 8080})
	cherrypy.engine.start()
	cherrypy.engine.block()