#include <winsock2.h>
#include <WS2tcpip.h>
#include <ctime>
#include <vector>
#include <conio.h>
#include <process.h>+
#include <stdio.h>
#include <list>
#include <chrono>

using namespace std;

#pragma comment( lib, "ws2_32.lib")

enum class ePacketType
{
	PlayerEnter,
	PlayerExit,
	PlayerConnect,
	PlayerDisconnect,
	TimeOut,
	GameStart,
	GameEnd
};

typedef struct _packetData
{
	int size;
	char* body;
} tPacketData;

char input[100] = "";
int i = 0;
char random;
SOCKET   hSocket;

vector<SOCKET> listSocket;
vector<const char*> listMsg;

int playerScore[2];
u_short playerChoice[2];
const char* printChoice[4] = { "선택안함", "가위", "바위", "보" };
bool gameStart = false;
bool gamePlaying = false;
bool gameEnd = false;
bool isUsed = false;
int playerChoiceCount = 0;
HANDLE g_updateThreadHandle;

LARGE_INTEGER timer, start, end1;
float deltaTime, elapsed;

template<typename T>
tPacketData WritePacket(ePacketType what, T& body);
int ReadPacket(char* body, int size, SOCKET s);
void SendAll(tPacketData str, SOCKET exceptclientSocket = -1);
unsigned int __stdcall Comm(void* _pArgs);
int Init();
void PrintBoard();
unsigned int __stdcall Update(void* _pArgs);

#define TIMEOUT			10.0f

int main(void)
{
	if (Init() == -1) return -1;

	SOCKADDR_IN		clientAddr;
	SOCKET clientSocket;
	int			sizeClientAddr = sizeof(clientAddr);

	srand(time(nullptr));

	std::puts("서버 실행중");

	unsigned int threadID;
	HANDLE hThread;

	while (1)
	{
		PrintBoard();

		clientSocket = accept(hSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
		if (clientSocket == SOCKET_ERROR)
		{
			printf("Failed accept() \n");
			return -1;
		}

		int listSocketSize = listSocket.size();

		if (listSocketSize < 1)
		{
			g_updateThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &Update, nullptr, 0, &threadID);
			if (g_updateThreadHandle == INVALID_HANDLE_VALUE) return 0;
		}

		listSocket.push_back(clientSocket);
		listSocketSize++;



		u_short client = (u_short)clientSocket;
		tPacketData data = WritePacket<u_short>(ePacketType::PlayerEnter, client);
		SendAll(data, clientSocket); 

		u_short size = sizeof(u_short) + (listSocketSize * sizeof(u_short));
		char* packet = new char[sizeof(u_short) + size]();
		char* temp = packet;
		memcpy(temp, &size, sizeof(u_short));
		temp += sizeof(u_short);
		u_short what = (u_short)ePacketType::PlayerConnect;
		memcpy(temp, &what, sizeof(u_short));
		temp += sizeof(u_short);
		for (int i = 0; i < listSocketSize; i++)
		{
			client = (u_short)listSocket[i];
			*(u_short*)temp = client;
			temp += sizeof(u_short);
		}

		send(clientSocket, packet, size + sizeof(u_short), 0);
		//for(int i=0;i<size+sizeof(u_short);i++)
		//	printf("%02x ", packet[i]);
		//puts("");

		if (!gameStart && listSocketSize >= 2)
		{
			gameStart = true;
		}

		delete[] data.body;

		hThread = (HANDLE)_beginthreadex(NULL, 0, &Comm, (void*)&clientSocket, 0, &threadID);
		if (hThread == INVALID_HANDLE_VALUE) return 0;
	}

	std::puts("서버 종료");

	closesocket(hSocket);
	WSACleanup();

	return 0;
}


template<typename T>
tPacketData WritePacket(ePacketType what, T& body)
{
	u_short size = sizeof(u_short) + sizeof(body);
	char* packet = new char[size + sizeof(u_short)];
	char* temp = packet;
	memcpy(temp, &size, sizeof(u_short));
	temp += sizeof(u_short);
	memcpy(temp, &what, sizeof(u_short));
	temp += sizeof(u_short);
	*(u_short*)temp = body;
	tPacketData data = { size + sizeof(u_short), packet };
	return data;
}

int ReadPacket(char* body, int size, SOCKET s)
{
	char* temp = body;
	u_short header = *(u_short*)temp;
	if (header != size - sizeof(u_short)) return -1;

	temp += sizeof(u_short);
	u_short type = *(u_short*)temp;
	temp += sizeof(u_short);

	if (type == (u_short)ePacketType::PlayerEnter) // 누군가 처음 들어왔을떄
	{

	}
	else if (type == (u_short)ePacketType::PlayerExit)
	{

	}
	else if (type == (u_short)ePacketType::PlayerConnect)
	{

	}
	else if (type == (u_short)ePacketType::PlayerDisconnect)
	{
		u_short t = 0;
		tPacketData data = WritePacket<u_short>(ePacketType::PlayerDisconnect, t);
		send(s, data.body, data.size, 0);
		int i = 0;

		vector<SOCKET>::iterator iter = listSocket.begin();
		vector<SOCKET>::iterator iterEnd = listSocket.end();
		vector<SOCKET>::iterator target;

		for (; iter != iterEnd; ++iter, i++)
		{
			if (*iter == s) // [0] or [1]이 나갈때만 0초로 초기화
			{
				target = iter;
				closesocket(s);
				CloseHandle(GetCurrentThread());
				if (i == 0 || i == 1)
					elapsed = 0.0f;

				if (i == 0) // [0]이 나간 경우 [1]에 있던 점수가 [0]으로 이동
				{
					playerScore[0] = playerScore[1];
					playerScore[1] = 0;
				}
			}
			else
			{
				u_short t = s;
				tPacketData data = WritePacket<u_short>(ePacketType::PlayerExit, t);
				send(s, data.body, data.size, 0);
			}
		}

		listSocket.erase(target);

		int listSocketSize = listSocket.size();
		if (listSocketSize < 2)
		{
			gamePlaying = false;
			elapsed = 0.0f;
			u_short t = 0;
			tPacketData data = WritePacket<u_short>(ePacketType::GameEnd, t);
			for (int i = 0; i < listSocketSize; i++)
				send(listSocket[i], data.body, data.size, 0);
		}
		PrintBoard();
	}
	else if (type == (u_short)ePacketType::TimeOut)
	{
		int i = 0;
		vector<SOCKET>::iterator iter = listSocket.begin();
		vector<SOCKET>::iterator iterEnd = listSocket.end();

		for (; iter != iterEnd; iter++, i++)
		{
			if (listSocket[i] == s)
			{
				playerChoice[i] = *(u_short*)temp;
				playerChoiceCount++;
				break;
			}
		}
	}
	return -1;
}

void PrintBoard()
{
	if (!isUsed)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,0 });
		isUsed = true;
		//vector<SOCKET>::iterator iterEnd = listSocket.end();
		int size = listSocket.size();
		system("cls");
		std::puts("================== 가위바위보 게임 ==================");
		//if (iter == iterEnd) puts("");
		if (size == 0) std::puts("");
		//for(int i=1; iter != iterEnd && i <= 2; iter++, i++)
		int i = 0;
		for (; i <= 1; i++)
			if (i <= (size - 1))
				printf("플레이어%d : %d --------> %s \t|\t 현재 승리한 횟수 : %d회\n", i + 1, (int)listSocket[i], printChoice[playerChoice[i]], playerScore[i]);
		std::puts("================== 대기열 ==================");
		if (size > 2)
		{
			vector<SOCKET>::iterator iter = listSocket.begin() + i;
			vector<SOCKET>::iterator endIter = listSocket.end();
			for (; iter != endIter; iter++) // for문 출력 도중 소켓 하나가 연결이 끊어지면 listSocket의 크기가 1 줄어드는데, 매번 listSocket.end() 호출
				printf("%d ", (int)*iter);
		}
		std::puts("\n============================================");
		//if (gamePlaying)
		//	printf("타이머 : %f\n", TIMEOUT - elapsed);
		isUsed = false;
	}
}

unsigned int __stdcall Comm(void* _pArgs)
{
	SOCKET s = *((SOCKET*)_pArgs);

	while (1)
	{
		int tempSize;
		char temp[100];
		tempSize = recv(s, temp, sizeof(temp), 0);

		if (tempSize != SOCKET_ERROR)
		{
			char* inputTemp = new char[tempSize];
			memcpy(inputTemp, &temp, tempSize);

			ReadPacket(inputTemp, tempSize, s);
		}
	}
}


void SendAll(tPacketData data, SOCKET exceptclientSocket)
{
	for (int i = 0; i < listSocket.size(); i++)
		if (exceptclientSocket != listSocket[i])
			send(listSocket[i], data.body, data.size, 0);
}

int Init()
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

	QueryPerformanceFrequency(&timer);
}

// 1 : 가위
// 2 : 바위
// 3 : 보
void Game()
{
	if (playerChoice[0] <= 0 || playerChoice[0] >= 4)
		playerChoice[0] = (rand() % 3) + 1;
	if (playerChoice[1] <= 0 || playerChoice[1] >= 4)
		playerChoice[1] = (rand() * rand() % 3) + 1;

	if (playerChoice[0] == 1)
	{
		if (playerChoice[1] == 2) playerScore[1]++;
		else if (playerChoice[1] == 3) playerScore[0]++;
	}
	else if (playerChoice[0] == 2)
	{
		if (playerChoice[1] == 1) playerScore[0]++;
		else if (playerChoice[1] == 3) playerScore[1]++;
	}
	else if (playerChoice[0] == 3)
	{
		if (playerChoice[1] == 1) playerScore[1]++;
		else if (playerChoice[1] == 2) playerScore[0]++;
	}
}

unsigned int __stdcall Update(void* _pArgs)
{
	while (1)
	{
		int listSocketSize = listSocket.size();

		if (listSocketSize <= 0) break;

		if (gameEnd)
		{
			QueryPerformanceCounter(&end1);
			deltaTime = (end1.QuadPart - start.QuadPart) / (float)timer.QuadPart;
			elapsed += deltaTime; // 서버에서 stop할 때까지 계속 측정

			if (elapsed >= TIMEOUT - 5) // 5초간 쉬는시간 후 
			{
				if (!gameStart && listSocketSize >= 2)
					gameStart = true;
				gameEnd = false;
				elapsed = 0.0f;
			}
		}

		if (gameStart && listSocketSize >= 2)
		{
			u_short soc;
			ePacketType type;
			tPacketData data;
			type = ePacketType::GameStart;

			for (int i = 0; i < listSocketSize; i++)
			{
				soc = (u_short)listSocket[i];
				data = WritePacket<u_short>(type, soc);
				send(listSocket[i], data.body, data.size, 0);
			}

			QueryPerformanceCounter(&start);
			gameStart = false;
			gamePlaying = true;
		}

		if (gamePlaying)
		{
			Sleep(1);
			QueryPerformanceCounter(&end1);
			deltaTime = (end1.QuadPart - start.QuadPart) / (float)timer.QuadPart;
			elapsed += deltaTime; // 서버에서 stop할 때까지 계속 측정


			if (elapsed >= TIMEOUT)
			{
				u_short soc;
				tPacketData data;
				ePacketType type;
				for (int i = 0; i < listSocketSize; i++) // SendAll 사용하기
				{
					soc = (u_short)listSocket[i];
					data = WritePacket<u_short>(ePacketType::TimeOut, soc);
					send(listSocket[i], data.body, data.size, 0);
				}
				elapsed = 0.0f;
				gameEnd = true;
				gamePlaying = false;

				while (playerChoiceCount < 2) { ; }
				playerChoiceCount = 0;
				Game();
			}
		}
		if (!isUsed)
		{
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,14 });
			printf("                               ");
			printf("%s타이머 : %.2f               \n", !gamePlaying ? "쉬는시간 " : "", (gamePlaying ? TIMEOUT - elapsed : 5.0f - elapsed));
		}
		start = end1;
		//PrintBoard();
	}

	CloseHandle(GetCurrentThread());
	return -1;
}