#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <ctime>
#include <windows.h>
#include <list>
#include <process.h>
#include <queue>

using namespace std;

// 나중에 들어온 클라는 지금까지 나눴던 데이터 10줄 정보 가져옴.

#pragma comment( lib, "ws2_32.lib")

list<const char*> conversationList;
bool isEntered = false;
int i = 0;
char input[100] = "";
SOCKET   hSocket;
bool isUsed = false;
bool isEnd = false;
//queue<float> queueMsgElapsed;

void PrintBoard()
{
	if (!isUsed)
	{ 
		isUsed = true;
		system("cls");
		for (int i = 0; i < 10 - conversationList.size(); i++) puts("");
		list<const char*>::iterator iter = conversationList.begin();
		list<const char*>::iterator iterEnd = conversationList.end();
		for (; iter != iterEnd; iter++)
			printf("%s\n", *iter);
		puts("=========================================================");
		printf("입력 : %s\n", input);
		isUsed = false;
	}
}

unsigned int __stdcall Comm(void* _pArgs)
{
	while (1)
	{
		int tempSize;
		char temp[100];
		tempSize = recv(hSocket, temp, sizeof(temp), 0);

		if (tempSize != SOCKET_ERROR)
		{
			char* inputTemp = new char[strlen(temp) + 1];
			strcpy_s(inputTemp, strlen(temp) + 1, temp);
			if (strcmp("▒", inputTemp) == 0)
			{
				isEnd = true;
				break;
			}
			conversationList.push_back(inputTemp);
			PrintBoard();
		}
	}
	return -1;
}

int main(void)
{
	WSADATA  wsaData;
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

	unsigned int threadID;
	HANDLE hThread;
	hThread = (HANDLE)_beginthreadex(NULL, 0, &Comm, nullptr, 0, &threadID);

	PrintBoard();

	srand(time(nullptr));

	LARGE_INTEGER timer, start, end;
	float deltaTime, elapsed = 0.0f;
	QueryPerformanceFrequency(&timer);

	while (1)
	{

		if (_kbhit())
		{
			char ch = _getch();
			if (ch == '\r')
			{
				if (strlen(input) > 0)
				{
					send(hSocket, input, strlen(input) + 1, 0);
					memset(input, 0, sizeof(input));
					//queueMsgElapsed.push(elapsed);
					i = 0;
				}
			}
			else if (ch == '\b')
			{
				if (i >= 1)
					input[--i] = 0;
			}
			else if (ch == 27) // esc키
			{
				send(hSocket, "▒", strlen("▒") + 1, 0);
			}
			else
				input[i++] = ch;
			PrintBoard();
		}

		if (isEnd) break;

		if (conversationList.size() > 10)
		{
			delete[] conversationList.front();
			conversationList.pop_front();
			PrintBoard();
		}
	}

	closesocket(hSocket);
	WSACleanup();
	CloseHandle(hThread);

	system("pause");
	return 0;
}