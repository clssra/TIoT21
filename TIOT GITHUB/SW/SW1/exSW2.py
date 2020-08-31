import cherrypy
import json

class Convertor():
    exposed = True

    def GET(self, *uri, **params):
        
        fout = open("convertor.json", "w")
        originalUnit = uri[1]
        targetUnit =  uri[2]
        originalValue = float(uri[0])
        
        dicty = {
                "originalValue": originalUnit,
                "originalUnit": originalValue,
                "targetUnit": targetUnit,

                }

       
        if(len(uri)!=3):
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
        
        dicty['newValue'] = newValue
        
        fout.write(json.dumps(dicty, indent=4))

        return json.dumps(dicty)


if __name__ == '__main__':
    
    conf={
            '/':{
                'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
                'tools.sessions.on': True
                }
    }
    cherrypy.tree.mount(Convertor(), '/converter', conf)
    cherrypy.engine.start()
    cherrypy.engine.block()

