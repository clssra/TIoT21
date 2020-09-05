import requests
import string
import json

url='http://127.0.0.1:8080'


if __name__=='__main__':
	while True:
		print('Cosa vuoi fare?\ni : info sul broker\nld : lista devices\nd : cerca device\nlu :lista utenti\nu: cerca utente\nq:uscita')
		x=input()
		print('')
		if x=='q':
			break
		elif x=='i':
			r=requests.get(url+'/info')
			print(json.dumps(r.json(),indent=4))
		elif x=='ld':
			r=requests.get(url+'/devices')
			print(json.dumps(r.json(),indent=4))
		elif x=='lu':
			r=requests.get(url+'/users')
			print(json.dumps(r.json(),indent=4))
		elif x=='d':
			ID=input('ID del device?')
			r=requests.get(url+'/devices?ID='+ID)
			print('')
			print(json.dumps(r.json(),indent=4))
		elif x=='u':
			ID=str(input("ID dell'utente?"))
			r=requests.get(url+'/users?ID='+ID)
			print('')
			print(json.dumps(r.json(),indent=4))
		else:
			print('Comando sconosciuto')
		print('')

