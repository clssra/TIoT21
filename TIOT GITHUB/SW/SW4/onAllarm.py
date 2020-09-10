def mailFile(name,surname,email, time):
	email=f"from: secure@home.com\nto: {email}\nobject: Allarm\nDear {name} {surname},\nIt seems that our sensors have noticed some presence in your home at: {time}. If it is you just ignore this email.\n\nSecure@home staff"
	file=open(email+".txt","w")
	file.write(email)
	file.close()
