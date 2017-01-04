/*
Simple UDP Server
*/
#include "stdafx.h"
#include<stdio.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer
#define SOCKLEN 64 //Max length of socket name
#define PORTLEN 8 //Max length of the port
#define DEFAULTPORT 27816 // Default port
int main()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	char sock[SOCKLEN];
	char portStr[PORTLEN];
	u_short port;
	WSADATA wsa;

	slen = sizeof(si_other);

	printf("Enter Port (%d):\n", DEFAULTPORT);
	gets_s(portStr);
	if (strlen(portStr) == 0)
	{
		port = DEFAULTPORT;
	}
	else {
		port = (u_short)strtoul(portStr, NULL, 0);
	}

	//Initialize winsock
	printf("\nInitializing Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialized.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		inet_ntop(AF_INET, &si_other.sin_addr, sock, SOCKLEN);
		printf("Received packet from %s:%d\n", sock, ntohs(si_other.sin_port));
		printf("RCV> %s\n", buf);
		printf("SND> %s\n", buf);

		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}