import cherrypy
import json

valuelist=[]

class Convertor():
    exposed = True

    def GET(self, *uri, **params):
        if (uri[0]=='log'):
            return json.dumps(valuelist, indent=4)
        
        fout = open("convertor.json", "w")
        if (not(('originalUnit' in params) and ('targetUnit' in params) and ('value' in params))):
            return 'Wrong name of params: they should be "originalUnit", "targetUnit" and "value".'
        originalUnit = params['originalUnit']
        targetUnit = params['targetUnit']
        originalValue = float(params['value'])
        if(len(params)!=3):
            return 'Wrong number of params.'
        if(originalUnit == 'C'):
            if(targetUnit == 'K'):
                newValue = originalValue + 273.15
            elif(targetUnit == 'F'):
                newValue = (originalValue * 9/5) + 32
            else:
                return 'Wrong param value: use "C", "K" or "F".'
        elif(originalUnit == 'K'):
            if(targetUnit == 'C'):
                newValue = originalValue - 273.15
            elif(targetUnit == 'F'):
                newValue = ((originalValue - 273.15) * 9/5) + 32
            else:
                return 'Wrong param value: use "C", "K" or "F".'
        elif(originalUnit == 'F'):
            if(targetUnit == 'C'):
                newValue = (originalValue - 32) * 5/9
            elif(targetUnit == 'K'):
                newValue = (originalValue - 32) * 5/9 + 273.15
            else:
                return 'Wrong param value: use "C", "K" or "F".'
        else:
            return 'Wrong param value: use "C", "K" or "F".'
        
        params['newValue'] = newValue
        
        fout.write(json.dumps(params, indent=4))

        return json.dumps(params)

    def POST(self, *uri):
        if uri[0]=='log':
            valuelist.append(json.loads(cherrypy.request.body.read()))


if __name__ == '__main__':
    
    conf={
            '/':{
                'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
                'tools.sessions.on': True
                }
    }
    cherrypy.tree.mount(Convertor(), '/', conf)
    cherrypy.config.update({'server.socket_host': '0.0.0.0'})
    cherrypy.config.update({'server.socket_port': 8080})
    cherrypy.engine.start()
    cherrypy.engine.block()
