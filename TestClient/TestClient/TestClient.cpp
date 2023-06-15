#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <ctime>
#include <windows.h>
#include <list>

using namespace std;

#pragma comment( lib, "ws2_32.lib")

list<const char*> conversationList;
int i = 0;
char input[100] = "";

void PrintBoard()
{
	system("cls");
	list<const char*>::iterator iter = conversationList.begin();
	list<const char*>::iterator iterEnd = conversationList.end();
	for (; iter != iterEnd; iter++)
		printf("%s\n", *iter);
	puts("=========================================================");
	printf("입력 : %s\n", input);
}

int main(void)
{
	WSADATA  wsaData;
	SOCKET   hSocket;
	SOCKADDR_IN  servAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed WSAStartup() \n");
		return -1;
	}

	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hSocket == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return -1;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
	servAddr.sin_port = htons(30002);

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("Failed connect() \n");
		return -1;
	}

	u_long nonBlockingMode = 1;
	ioctlsocket(hSocket, FIONBIO, &nonBlockingMode);

	PrintBoard();

	srand(time(nullptr));
	char random;

	while (1)
	{
		int tempSize;
		char temp[100];
		tempSize = recv(hSocket, temp, sizeof(temp), 0);

		if (_kbhit())
		{
			char ch = _getch();
			if (ch == '\r')
			{
				if (strlen(input) > 0)
				{
					send(hSocket, input, strlen(input) + 1, 0);
					memset(input, 0, sizeof(input));
					i = 0;
				}
			}
			else if (ch == '\b')
			{
				if (i >= 1)
					input[--i] = 0;
			}
			else
				input[i++] = ch;
			PrintBoard();
		}

		// test
		/*
		random = rand() % 26 + 'a';
		char test[5];
		sprintf_s(test, sizeof(test), "%c", random);
		send(hSocket, test, strlen(test) + 1, 0);
		*/

		if (tempSize != SOCKET_ERROR)
		{
			char* inputTemp = new char[strlen(temp) + 1];
			strcpy_s(inputTemp, strlen(temp) + 1, temp);
			conversationList.push_back(inputTemp);
			PrintBoard();
		}

		if (conversationList.size() > 10)
		{
			delete[] conversationList.front();
			conversationList.pop_front();
			PrintBoard();
		}


		/*
		if (tempSize == -1)
		{
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				printf("수신안함\n");
				continue;
			}
			else
			{
				printf("recv() Error  \n");
				return -1;
			}
		}
		*/


		
	}

	closesocket(hSocket);
	WSACleanup();

	system("pause");
	return 0;
}