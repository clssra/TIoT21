import cherrypy
import json

class Convertor():
    exposed = True

    def GET(self, *uri, **params):
        
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


if __name__ == '__main__':
    
    conf={
            '/':{
                'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
                'tools.sessions.on': True
                }
    }
    cherrypy.tree.mount(Convertor(), '/', conf)
    cherrypy.engine.start()
    cherrypy.engine.block()


