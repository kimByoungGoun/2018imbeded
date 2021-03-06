// communication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUF_SIZE 1024

using namespace std;

void ErrorHandling(const char* message);


int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;
	TIMEVAL timeout;
	fd_set reads, cpyReads;

	int adrSz;
	int strLen, fdNum, i;
	char buf[BUF_SIZE];

	int inputPort;
	cout << "input port : ";
	cin >> inputPort ;

	int szClntAddr;
	char message[] = "Hello";
	
	//////////////////socket setting for windows/////////////////////////
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");
	/////////////////////////////////////////////////////////////////////
	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//ip 할당. 아무거나
	servAddr.sin_port = htons(inputPort);		//port 할당. 입력으로부터
	//////////////bind, listen, accept/////////////////////////////////////////////////
	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	FD_ZERO(&reads);
	FD_SET(hServSock, &reads);
	while(1)
	{
		cpyReads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
			break;

		if (fdNum == 0)
			continue;

		for (i = 0; i < reads.fd_count; i++)
		{
			if (FD_ISSET(reads.fd_array[i], &cpyReads))
			{
				if (reads.fd_array[i] == hServSock)
				{
					adrSz = sizeof(clntAddr);
					hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &adrSz);
					FD_SET(hClntSock, &reads);
					cout << "connected client : " << hClntSock << endl;
				}
				else
				{
					strLen = recv(reads.fd_array[i], buf, BUF_SIZE-1,0);
					if (strLen == 0)
					{
						FD_CLR(reads.fd_array[i], &reads);
						closesocket(cpyReads.fd_array[i]);
						cout << "closed client " << cpyReads.fd_array[i] << endl;
					}
					else
					{
						send(reads.fd_array[i], buf, strLen, 0);
					}
				}
			}
		}

		
	}
	closesocket(hServSock);
	WSACleanup();
	
	return 0;
}

void ErrorHandling(const char* message)
{
	cout << message << endl;
	exit(1);
}
//너무 주저리주저리하다. 소켓 하나당 클래스 객체 하나로 만들고 바인드, 리슨 등을 기능으로 묶자