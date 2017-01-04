/*
Simple udp client
*/
#include "stdafx.h"
#include<stdio.h>
#include<winsock2.h>
#include<WS2tcpip.h>
#include<string.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define SERVERLEN 16 // Max length of server IP
#define PORTLEN 8 // Max length of port
#define BUFLEN 512  //Max length of buffer
#define DEFAULTPORT 27816 //default port
int main(void)
{
	const char DEFAULTSERVER[16] = "127.0.0.1";
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	char server[SERVERLEN];
	char portStr[PORTLEN];
	u_short port;
	WSADATA wsa;

	printf("Enter IP Address (%s): \n", DEFAULTSERVER);
	gets_s(server);
	if (strlen(server) == 0)
	{
		memcpy(server, DEFAULTSERVER, strlen(DEFAULTSERVER));
	}
	printf("Enter Port (%d): \n", DEFAULTPORT);
	gets_s(portStr);
	if (strlen(portStr) == 0)
	{
		port = DEFAULTPORT;
	}
	else
	{
		port = (u_short)strtol(portStr, NULL, 0);
	}

	//Initialize winsock
	printf("\nInitializing Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialized.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	//si_other.sin_addr.S_un.S_addr =
	inet_pton(AF_INET, server, &si_other.sin_addr.S_un.S_addr);

	//start communication
	while (1)
	{
		printf("Enter message : ");
		gets_s(message);
		printf("SND> %s\n", message);
		//send the message
		if (sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//		puts(buf);
		printf("RCV> %s\n", buf);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}