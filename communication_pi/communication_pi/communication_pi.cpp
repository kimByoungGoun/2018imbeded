// communication_pi.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>

#define BUF_SIZE 1024
using namespace std;

void ErrorHandling(const char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	char message[BUF_SIZE];
	int strLen;
	SOCKADDR_IN servAddr;

	if (argc != 3)
	{
		cout << "Usage : " << argv[0] << "<port>" << endl;
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");
	else
		cout << "connected........" << endl;

	while(1)
	{
		cout << "input message(Q to quit): ";
		cin >> message;
		if (message == "q" || message == "Q")
		{
			break;
		}
		send(hSocket, message, strlen(message), 0);
		strLen = recv(hSocket, message, BUF_SIZE - 1, 0);
		message[strLen] = 0;
		cout << "Message from server : " << message << endl;

		
	}
	closesocket(hSocket);
	WSACleanup();
	return 0;
	
}

void ErrorHandling(const char* message)
{
	cout << message << endl;
	exit(1);
}
