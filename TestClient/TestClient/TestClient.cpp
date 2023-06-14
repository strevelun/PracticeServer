#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <ctime>
#include <windows.h>

#pragma comment( lib, "ws2_32.lib")

const char* printChoice[3] = { "가위", "바위", "보" };

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

	LARGE_INTEGER timer, start, end;
	float deltaTime, timeLeft;
	QueryPerformanceFrequency(&timer);

	float elapsed;

	while (1)
	{
		int recvSize;
		char recvData[10];
		recvSize = recv(hSocket, recvData, sizeof(recvData), 0);

		if (recvSize == -1)
		{
			printf("recv() Error  \n");
			return -1;
		}

		int game = recvData[0] - '0';

		if (game == 0)
		{
			int input = 0;
			elapsed = 0.0f;

			while (1)
			{
				QueryPerformanceCounter(&start);
				if (_kbhit())
				{
					input = _getch() - '0';
				}

				system("cls");


				QueryPerformanceCounter(&end);
				deltaTime = (end.QuadPart - start.QuadPart) / (float)timer.QuadPart;

				elapsed += deltaTime;
				timeLeft = 3.0 - elapsed;

				if (timeLeft < 0.0)
				{
					if(input == 0)
						input = rand() % 3 + 1;
					break;
				}

				const char* temp = "없음";

				if (input > 0 && input < 4)
					temp = printChoice[input - 1];

				printf("%.1f초 남음, 현재 선택한거 : %s", timeLeft, temp);
			}

			char num[10];
			sprintf_s(num, sizeof(num), "%d", input);
			send(hSocket, num, strlen(num) + 1, 0);

			char temp[100];
			recv(hSocket, temp, sizeof(temp), 0);
			puts(temp);

		}
		else if (game == 1)
			break;
	}

	char result[20];
	recv(hSocket, result, sizeof(result), 0);
	puts(result);

	closesocket(hSocket);
	WSACleanup();

	system("pause");
	return 0;
}