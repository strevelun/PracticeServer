#include <winsock2.h>
#include <WS2tcpip.h>
#include <ctime>
#include <vector>
#include <conio.h>
#include <process.h>
#include <stdio.h>
#include <list>

using namespace std;

#pragma comment( lib, "ws2_32.lib")

list<const char*> conversationList;
char input[100] = "";
int i = 0;
char random;
SOCKET   hSocket;

vector<SOCKET> listSocket;
struct tData { int loc; SOCKET soc; };

void SendAll(char* str);

unsigned int __stdcall Comm(void* _pArgs)
{
	tData* data = (tData*)_pArgs;
	SOCKET s = data->soc;
	int loc = data->loc;

	while (1)
	{
		int tempSize;
		char temp[100];
		tempSize = recv(s, temp, sizeof(temp), 0);

		if (tempSize != SOCKET_ERROR)
		{
			char* inputTemp = new char[strlen(temp) + 1];
			strcpy_s(inputTemp, strlen(temp) + 1, temp);
			conversationList.push_back(inputTemp);
			if (strcmp("▒", inputTemp) == 0)
			{
				CloseHandle(GetCurrentThread());
				send(s, "▒", strlen("▒") + 1, 0);
				closesocket(s);
				listSocket.erase(listSocket.begin() + loc);
				printf("================ %d소켓 제거\n", (int)s);
				break;
			}
			printf("%d소켓으로부터 메시지 : %s\n", (int)s, inputTemp);
			SendAll(inputTemp);
			if (conversationList.size() > 10)
			{
				delete[] conversationList.front();
				conversationList.pop_front();
			}
		}
	}
	return -1;
}


void SendAll(char* str)
{
	for(int i=0;i<listSocket.size(); i++)
		send(listSocket[i], str, strlen(str) + 1, 0);
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
	if (listen(hSocket, 3) == SOCKET_ERROR)
	{
		printf("listen Error \n");
		return -1;
	}

	SOCKADDR_IN		clientAddr;
	SOCKET clientSocket;
	int			sizeClientAddr = sizeof(clientAddr);

	puts("서버 실행중");


	while (1)
	{
		clientSocket = accept(hSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
		if (clientSocket == SOCKET_ERROR)
		{
			printf("Failed accept() \n");
			return -1;
		}

		listSocket.push_back(clientSocket);
		int loc = listSocket.size() - 1;
		printf("================ %d 소켓 접속\n", (int)clientSocket);

		tData data;
		data.loc = loc;
		data.soc = clientSocket;

		unsigned int threadID;
		HANDLE hThread;
		hThread = (HANDLE)_beginthreadex(NULL, 0, &Comm, (void*)&data, 0, &threadID);
		if (hThread == INVALID_HANDLE_VALUE) return 0;
	}

	puts("서버 종료");

	closesocket(hSocket);
	WSACleanup();

	return 0;
}