#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stack>
#include <queue>
#include <ctime>
#include <list>
#include <conio.h>

using namespace std;

#pragma comment( lib, "ws2_32.lib")

list<const char*> conversationList;
char input[100] = "";
int i = 0;
char random;

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

void Test(SOCKET& clientSocket)
{
	// test
	random = rand() % 26;
	char test[5];
	sprintf_s(test, sizeof(test), "%d", random);
	char* inputTemp = new char[strlen(test) + 1];
	strcpy_s(inputTemp, strlen(test) + 1, test);
	conversationList.push_back(inputTemp);
	send(clientSocket, test, strlen(test) + 1, 0);
	if (conversationList.size() > 10)
	{
		delete[] conversationList.front();
		conversationList.pop_front();
		PrintBoard();
	}
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

	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hSocket == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return -1;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
	servAddr.sin_port = htons(30002);
	if (bind(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("Binding Error \n");
		return -1;
	}
	if (listen(hSocket, 2) == SOCKET_ERROR)
	{
		printf("listen Error \n");
		return -1;
	}

	SOCKADDR_IN		clientAddr;
	SOCKET			clientSocket;
	int			sizeClientAddr = sizeof(clientAddr);
	clientSocket = accept(hSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
	if (clientSocket == SOCKET_ERROR)
	{
		printf("Failed accept() \n");
		return -1;
	}

	u_long nonBlockingMode = 1;
	ioctlsocket(clientSocket, FIONBIO, &nonBlockingMode);

	PrintBoard();

	srand(time(nullptr));

	LARGE_INTEGER timer, start, end;
	float deltaTime, elapsed = 0.0f;
	QueryPerformanceFrequency(&timer);

	while (1)
	{
		QueryPerformanceCounter(&start);
		int tempSize;
		char temp[100];
		tempSize = recv(clientSocket, temp, sizeof(temp), 0);

		if (_kbhit())
		{
			char ch = _getch();
			if (ch == '\r')
			{
				if (strlen(input) > 0)
				{
					char* inputTemp = new char[strlen(input) + 1];
					strcpy_s(inputTemp, strlen(input) + 1, input);
					conversationList.push_back(inputTemp);
					send(clientSocket, input, strlen(input) + 1, 0);
					memset(input, 0, sizeof(input));
					i = 0;
					if (conversationList.size() > 10)
					{
						delete[] conversationList.front();
						conversationList.pop_front();
					}
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
		/*
		if (elapsed >= 0.1f)
		{
			Test(clientSocket);
			elapsed = 0.0f;
			PrintBoard();
		}*/

		if (tempSize != SOCKET_ERROR)
		{
			char* inputTemp = new char[strlen(temp) + 1];
			strcpy_s(inputTemp, strlen(temp) + 1, temp);
			conversationList.push_back(inputTemp);
			send(clientSocket, temp, strlen(temp) + 1, 0);
			if (conversationList.size() > 10)
			{
				delete[] conversationList.front();
				conversationList.pop_front();
			}
			PrintBoard();
		}


		QueryPerformanceCounter(&end);
		deltaTime = (end.QuadPart - start.QuadPart) / (float)timer.QuadPart;
		elapsed += deltaTime;


		/*
		if (recvSize == -1)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
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

	closesocket(clientSocket);
	closesocket(hSocket);
	WSACleanup();

	return 0;
}