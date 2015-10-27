# Author : Georgios Karapanagos - gkarapanagos
# Date : 09/14/2015
# make httpClient -> create the client that sends the HTTP requests

CFLAGS = -g

all: httpClient

httpClient: httpGet.o
	g++ $(CPPFLAGS) -m32 -Wall -lm httpGet.o -o httpClient

httpGet.o: httpGet.cpp
	g++ $(CPPFLAGS) -m32 -Wall -c httpGet.cpp
	
clean:
	rm -f httpClient.o httpClient
