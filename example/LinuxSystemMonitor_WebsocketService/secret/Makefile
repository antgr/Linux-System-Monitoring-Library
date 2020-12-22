

pem:
	openssl genrsa 4096 > key.pem
	openssl req -new -key key.pem | openssl x509 -days 365 -req -signkey key.pem > cert.pem