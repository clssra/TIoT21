import cherrypy
import json

class Convertor():
    exposed = True

    def PUT(self, *uri, **params):
        
        body = cherrypy.request.body.read()
        json_body = json.loads(body.decode('utf-8'))
        
        if (not(('originalUnit' in json_body) and ('targetUnit' in json_body) and ('values' in json_body))):
            return 'Wrong name of params: they should be "originalUnit", "targetUnit" and "values".'

        originalUnit = json_body['originalUnit']
        targetUnit = json_body['targetUnit']
        
        if(len(json_body)!=3):
            return 'Wrong number of params.'

        fout = open("convertor.json", "w")
        i=0
        newValues = []
        for v in json_body['values']:
            
            originalValue = v
            if(originalUnit == 'C'):
                if(targetUnit == 'K'):
                    newValues.append(originalValue + 273.15)
                elif(targetUnit == 'F'):
                    newValues.append((originalValue * 9/5) + 32)
                else:
                    return 'Wrong param value: use "C", "K" or "F".'
            elif(originalUnit == 'K'):
                if(targetUnit == 'C'):
                    newValues.append(originalValue - 273.15)
                elif(targetUnit == 'F'):
                    newValues.append(((originalValue - 273.15) * 9/5) + 32)
                else:
                    return 'Wrong param value: use "C", "K" or "F".'
            elif(originalUnit == 'F'):
                if(targetUnit == 'C'):
                    newValues.append((originalValue - 32) * 5/9)
                elif(targetUnit == 'K'):
                    newValues.append((originalValue - 32) * 5/9 + 273.15)
                else:
                    return 'Wrong param value: use "C", "K" or "F".'
            else:
                return 'Wrong param value: use "C", "K" or "F".'
            
        json_body['newValues'] = newValues
    

        return json.dumps(json_body)


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



