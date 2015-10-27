/**
Author: Georgios Karapanagos - gkarapanagos
Date: 09/25/2015
CS3516 - Programming Assignment 2

Socket code taken from the "TCP/IP Sockets in C" book by Donahoo and Calvert.
The original code can be found at: http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoClient.c
The code to use gettimeoftheday() came from http://www.ccplusplus.com/2011/11/gettimeofday-example.html
The idea of how to use getsockopt() was inspired by http://linuxgazette.net/136/pfeiffer.html
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
struct hostent *hp;



int main (int argc, char* argv[]){
	// Booleans for the 5 possible options
	bool fOption = false;
	bool nfOption = false;
	bool pingOption = false;
	bool pktOption = false;
	bool infoOption = false;

	// Initialize necessary variables
	struct timeval currentTime;										/* Struct used to measure ping */
	int Socket; 													/* Socket descriptor */
	struct sockaddr_in ServAddr; 									/* Echo server address */
	char Buffer[10000]; 											/* Buffer for echo string */
	int bytesRcvd = 0, totalBytesRcvd = 0; 							/* Bytes read in single recv() and total bytes read */
	char* outputFilePath = "web.txt";								/* Output file, default value being web.txt */
	double tBefore;													/* Current time before connecting */
	double tAfter;													/* Current time after connecting */
	// double tBefore1;												/* This is used to do the 3 object download graphs */
	stringstream respStr;											/* Holds the HTTP response string stream */
	string respSt;													/* Holds the HTTP response string*/
	char* host = argv[1];											/* Target web server */
	int bytes[10000];												/* Holds the bytes received, up to 10K */	
	double times[10000];										 	/* Holds the time it took for each packet to be received */
	
	for (int i = 0; i < 9999; i++)
		{
			bytes[i] = -1;
		}

	// Remove https:// and http:// if found, because gethostbyname doesn't work with these included
	string temp(host);
	if(temp.find("https://")==0) memcpy(host, &host[8], temp.length());
	if(temp.find("http://")==0) memcpy(host, &host[7], temp.length());

	/* Construct the server address structure */
	memset(&ServAddr, 0, sizeof(ServAddr)); 						/* Zero out structure */
	ServAddr.sin_family = AF_INET; 									/* Internet address family */

	// Remove port number from host string
	temp = host;
	int pos = temp.find(":");
	int pos2;
	int port = 80;
	if(pos > 0)	{
		temp = temp.substr(pos, temp.length());
		pos2 = temp.find(".") + pos;
		string temp2(host);
		temp2 = temp2.substr(pos+1, pos2-pos-1);
		port = atoi((char*)temp2.c_str());
		ServAddr.sin_port = htons(port); 							/* Server port */

		temp = host;
		temp = temp.substr(0, pos) + temp.substr(pos2, temp.length());
	}
	else		ServAddr.sin_port = htons(80); 						/* Server port */

	

	// Get the options called
	for (int i = 2; i < argc; i++) { 	// Iterate over argv[] to get the options called. Starting on 1 because stored in argv[0] is the server
				string temp(argv[i]);
	                if (temp.find("-f") == 0) {
	                    fOption = true;
	                    nfOption = false;
	                    // Make sure we aren't done yet
	                    if (i != argc -1)
	                    	outputFilePath = argv[i + 1];
	                } else if (temp.find("-nf") == 0) {
	                	nfOption = true;
	                	fOption = false;
	                } else if (temp.find("-ping") == 0) {
	                	pingOption = true;
	                } else if (temp.find("-pkt") == 0) {
	                	pktOption = true;
	                	nfOption = true;
	                } else if (temp.find("-info") == 0) {
	                	infoOption = true;
	            }}

	ofstream outputFile;
    outputFile.open(outputFilePath);


	/* Create a reliable, stream socket using TCP */
	if ((Socket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		cout << "Socket failed!";


	temp = host;
	pos = temp.find("/");
	if(pos > 0) {
		temp = temp.substr(0,pos);
	}
	// Get Server IP Addres
	char * tempHost = (char*)temp.c_str();
	    if((hp=gethostbyname(tempHost))==NULL){
	    	cout << "Couldn't get the IP.\n";
	        return 0;
	  }
	// host = "www.piriform.com/ccleaner/download/standard";  
	bcopy(hp->h_addr,(char *)&ServAddr.sin_addr,hp->h_length);		/* Server IP address */

	
	// gettimeofday(&currentTime, NULL);								/* This is used to do the 3 object download graphs */
    // tBefore1 =currentTime.tv_sec+(currentTime.tv_usec/1000000.0);	/* This is used to do the 3 object download graphs */

    if(pingOption){
        gettimeofday(&currentTime, NULL);
        tBefore =currentTime.tv_sec+(currentTime.tv_usec/1000000.0);}

    // Connect to the server
    if (connect(Socket, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0){
    	cout << "Could not connect.\n";
    	return 0;}

    if(pingOption){
    	gettimeofday(&currentTime, NULL);
    	tAfter =currentTime.tv_sec+(currentTime.tv_usec/1000000.0);
    	cout << "Ping was measured to be: " << 1000* (tAfter - tBefore) << " milliseconds.\n";
    }

    // Create the GET request string
    string get = "GET / HTTP/1.1\r\nHost: ";
    get += host;
    get += "\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.3) Gecko/20060426 Firefox/1.5.0.3\r\n";
    get += "Accept:text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Language: en-us,en;q=0.5\r\n";
    get += "Accept-Encoding: gzip,deflate\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\nKeep-Alive: 300\r\nConnection: close\r\n\r\n";
    char * Get =(char*) get.c_str();

    // Send the HTTP GET request
    send(Socket, Get , strlen(Get) ,0);

    

    int k = 0;

    // Get time before receive
    gettimeofday(&currentTime, NULL);
    tBefore =currentTime.tv_sec+(currentTime.tv_usec/1000000.0);
    // Receive the HTTP OK response
    while ((bytesRcvd = recv(Socket, Buffer, 9999 ,0)) > 0){
    	// Get time after receive
    	gettimeofday(&currentTime, NULL);
    	tAfter =currentTime.tv_sec+(currentTime.tv_usec/1000000.0);
    	// Save number of bytes received
    	// cout << "Packet Size: " << totalBytesRcvd << "\n"; 								/* This is used to do the 3 object download graphs */
    	// cout << "Time : " << tAfter - tBefore1 << "\n";								/* This is used to do the 3 object download graphs */
    	if(pktOption) bytes[k] += bytesRcvd;
    	// Save time it took for bytes to be received
    	times[k] = tAfter - tBefore;
    	// Count number of total bytes received
        totalBytesRcvd += bytesRcvd;
        // Add HTTP response to the response string
        respStr << Buffer;
        k++;
        // Get time before receive
        gettimeofday(&currentTime, NULL);
    	tBefore =currentTime.tv_sec+(currentTime.tv_usec/1000000.0);
    }

    // cout << "Total bytes received are: " << totalBytesRcvd << "\n";
 	int max = -1;
    if(pktOption){
    	int i = 0;
    	while(bytes[i] >= 0 && i < 9999){
    		if(max < bytes[i])	max = bytes[i];
    		cout << bytes[i] << " bytes received in " << 1000 * times[i] << " milliseconds.\n";
    		i++;
    	}
    	cout << "The maximum packet size received is: " << max << " bytes.\n";
    }


    // Take the part of the HTTP response up to "Connection: close" and put it in a string
    respSt = respStr.str();
    int endOfStr = respSt.find("Connection: close");
    respSt = respSt.substr(0, endOfStr+18);

    // Print out the HTTP response string
    if(!nfOption)
    	outputFile << respSt;

    // Implement the -info option
    // Create a tcp_info struct
    tcp_info tcpInfo;
    int sockOptSize = sizeof(tcpInfo);
    if(infoOption){
    	// Call getsockopt
		k = getsockopt(Socket, SOL_TCP, TCP_INFO, (void *)&tcpInfo, (socklen_t *) &sockOptSize) ;
		// Output results
		cout << "The round trip time was: " << tcpInfo.tcpi_rtt/1000; 
		cout << " milliseconds.\nThe round trip time variance was: " << tcpInfo.tcpi_rttvar/1000 << " milliseconds.\n";
    }
    // Close the output file
    outputFile.close();

    // Close the socket
    close(Socket);

    return 1;
}
