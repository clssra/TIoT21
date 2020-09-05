import requests
import json
import random
import time

devices={}

if __name__=='__main__':
	while True:
		
		ID=random.randint(0,10)
		timestamp=time.time()
		data={'bn':ID, 'e':[{'n':'lightning' ,'u':'amph','t':timestamp,'v':ID*10}]}
		devices.update(data)
		r=requests.post('http://127.0.0.1:8080/devices',json.dumps(data))
		time.sleep(6)
