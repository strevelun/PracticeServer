#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stack>
#include <queue>
#include <ctime>

using namespace std;

#pragma comment( lib, "ws2_32.lib")

int serverCount = 0;
int clientCount = 0;

const char* printChoice[3] = { "가위", "바위", "보" };

int Add(char* str)
{
	int len = strlen(str);
	stack<int> s;
	queue<int> numbers;
	queue<char> ops;
	int a = 0;

	for (int i = 0; i <= len; i++)
	{
		if (i >= len || str[i] < '0' || str[i] > '9')
		{
			int j = 1, result = 0;
			int temp;
			while (!s.empty())
			{
				temp = s.top() * j;
				result += temp;
				j *= 10;
				s.pop();
			}

			numbers.push(result);

			if (i >= len) break;

			switch (str[i])
			{
			case '+':
			case '-':
				ops.push(str[i]);
				break;
			}
			continue;
		}
		s.push(str[i] - '0');
	}

	a = numbers.front();
	numbers.pop();

	while (!numbers.empty())
	{
		int t = numbers.front();
		numbers.pop();

		switch (ops.front())
		{
		case '+':
			a += t;
			break;
		case '-':
			a -= t;
			break;
		}
		ops.pop();
	}

	return a;
}

// 1 : 가위
// 2 : 바위
// 3 : 보
void Game(int server, int client)
{
	if (server == client)
		printf("무승부! === ");

	if (server == 1)
	{
		if (client == 2) clientCount++;
		else if (client == 3) serverCount++;
	}
	else if (server == 2)
	{
		if (client == 1) serverCount++;
		else if (client == 3) clientCount++;
	}
	else if (server == 3)
	{
		if (client == 1) clientCount++;
		else if (client == 2) serverCount++;
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

	clock_t start, finish;
	double elapsed;
	srand(time(NULL));
	int isGameEnd = 0; // 0은 안끝남

	while (1)
	{
		elapsed = 0.0;
		int choice = rand() % 3 + 1;

		char game[10];
		sprintf_s(game, sizeof(game), "%d", isGameEnd);
		send(clientSocket, game, strlen(game) + 1, 0); // 시작했다고 send

		if (isGameEnd == 1) break;


		int recvSize;
		char recvData[10];
		recvSize = recv(clientSocket, recvData, sizeof(recvData), 0);
		if (recvSize == -1)
		{
			printf("recv() Error  \n");
			return -1;
		}


		int counter = recvData[0] - '0'; // 클라이언트가 선택한 숫자
		

		Game(choice, counter);

		if (serverCount >= 3 || clientCount >= 3) isGameEnd = 1;

		char temp[100];
		sprintf_s(temp, sizeof(temp), "결과! 선택 -> (서버:%s, 클라:%s) | 결과 -> (서버:%d, 클라:%d)", printChoice[choice - 1], printChoice[counter - 1], serverCount, clientCount);
		puts(temp);
		send(clientSocket, temp, strlen(temp) + 1, 0);

		Sleep(3000);
	}

	char result[20];
	if (serverCount > clientCount)
		sprintf_s(result, sizeof(result), "%s", "서버 승!");
	else
		sprintf_s(result, sizeof(result), "%s", "클라 승!");

	send(clientSocket, result, strlen(result) + 1, 0);
	puts(result);

	closesocket(clientSocket);
	closesocket(hSocket);
	WSACleanup();

	return 0;
}