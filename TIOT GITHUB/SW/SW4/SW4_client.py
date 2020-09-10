import random
import requests
import json


def intro():
	while True:
		print('Benvenuto nel client del Lab04, inserisci le seguenti informazioni per registrarti al programma di sensori')
		mail=input('mail addr: ')
		nome=input('name: ')
		cognome=input('surname: ')
		ID=random.randint(0,10000)
		print(f'Queste informazioni sono corrette?(y/n)\n e-mail:{mail},\n nome:{nome},\n cognome={cognome}')
		y=input()
		if y=='y':
			return ID, mail, nome, cognome

def connect(res):
	data={'ID':res[0], 'email':res[1], 'name':res[2], 'surname':res[3]}
	r=requests.post("http://127.0.0.1:8080/users",json.dumps(data))
	return


if __name__ == '__main__':
	result=intro()
	connect(result)
	
