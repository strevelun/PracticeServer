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

const char* printChoice[4] = { "선택안함", "가위", "바위", "보" };
queue<const char*> recvList;
vector<SOCKET> listSocket;
bool isEntered = false;
char input;
SOCKET   hSocket;
HANDLE hThread;
bool isUsed = false;
bool isEnd = false;

int player;
bool gameStart = false;
bool gamePlaying = false;

LARGE_INTEGER timer, start, end1;
float deltaTime, elapsed;

template<typename T>
tPacketData WritePacket(ePacketType what, T& body);
int ReadPacket(char* body, int size);
void PrintBoard();
unsigned int __stdcall Comm(void* _pArgs);
void Input();
void Update();
int Init();

#define TIMEOUT			10.0f

int main(void)
{
	if (Init() == -1) return -1;

	while (1)
	{
		if (gameStart)
		{
			QueryPerformanceCounter(&start);
			gameStart = false;
			gamePlaying = true;
		}

		if (gamePlaying)
		{
			QueryPerformanceCounter(&end1);
			deltaTime = (end1.QuadPart - start.QuadPart) / (float)timer.QuadPart;
			elapsed += deltaTime; // 서버에서 stop할 때까지 계속 측정
			start = end1;

			if (!isUsed)
			{
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,14 });
				printf("                               ");
				printf("타이머 : %.2f\n", TIMEOUT - elapsed);
			}
		}

		Input();

		if (isEnd) break;
	}

	closesocket(hSocket);
	WSACleanup();
	CloseHandle(hThread);
	system("pause");

	return 0;
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

	srand(time(nullptr));
	unsigned int threadID;
	hThread = (HANDLE)_beginthreadex(NULL, 0, &Comm, nullptr, 0, &threadID);


	QueryPerformanceFrequency(&timer);
}

void PrintBoard()
{

	if (!isUsed)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,0 });
		isUsed = true;
		int size = listSocket.size();

		system("cls");
		printf("본인 : %d\n", player);
		puts("================== 가위바위보 게임 ==================");
		if (size == 0) puts("");
		int i = 0;
		for (; i <= 1; i++)
			if (i <= (size - 1))
				printf("플레이어%d : %d\n", i + 1, (int)listSocket[i]);
		puts("================== 대기열 ==================");
		for (; i < size; i++)
			printf("%d ", (int)listSocket[i]);
		puts("\n============================================");
		if (gamePlaying)
		{
			//printf("\t\t타이머 : %f\n", TIMEOUT - elapsed);
			int ip = input == 0 ? 0 : input - '0';
			printf("입력 : %s", printChoice[ip]);
		}
		isUsed = false;
	}
}

unsigned int __stdcall Comm(void* _pArgs)
{
	while (1)
	{
		int tempSize;
		char temp[100] = { 0 };
		tempSize = recv(hSocket, temp, sizeof(temp), 0);

		if (tempSize != SOCKET_ERROR)
		{
			char* inputTemp = new char[tempSize];
			memcpy(inputTemp, &temp, tempSize);

			int type = ReadPacket(inputTemp, tempSize);

			//for (int i = 0; i < tempSize; i++)
			//	printf("%02x ", inputTemp[i]);
			//puts("");

			if (type == (int)ePacketType::PlayerDisconnect)
			{
				isEnd = true;

				break;
			}

			PrintBoard();
		}
	}
	return -1;
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
	memcpy(temp, &body, sizeof(body));
	//temp += sizeof(body);
	tPacketData data = { size + sizeof(u_short), packet };
	return data;
}

// char* packet , recvedSize
int ReadPacket(char* body, int size)
{
	int count = 0;
	char* temp = body;  // packet



	while (count + sizeof(u_short) < size)
	{
		// size 
		u_short header = *(u_short*)temp;
		count += header;
		temp += sizeof(u_short);

		u_short type = *(u_short*)temp;
		temp += sizeof(u_short);
		if (type == (u_short)ePacketType::PlayerEnter) // 누군가 처음 들어왔을떄
		{
			int id = *(u_short*)temp;
			listSocket.push_back(id);
		}
		else if (type == (u_short)ePacketType::PlayerConnect) // 가장 마지막에 있는게 나 자신
		{
			int listSize = (header - sizeof(u_short)) / sizeof(u_short);
			int id = -1;
			for (int i = 0; i < listSize; i++)
			{
				id = *(u_short*)temp;
				listSocket.push_back(id);
				temp += sizeof(u_short);
			}
			player = id;
			continue;
		}
		else if (type == (u_short)ePacketType::PlayerDisconnect)
		{
			return type;
		}
		else if (type == (u_short)ePacketType::PlayerExit)
		{
			SOCKET disconnectedSocket = *(u_short*)temp;
			vector<SOCKET>::iterator iter = listSocket.begin();
			vector<SOCKET>::iterator iterEnd = listSocket.end();

			for (; iter != iterEnd; )
			{
				if (*iter == disconnectedSocket)
				{
					listSocket.erase(iter);
					break;
				}
			}
			if (listSocket.size() < 2) gamePlaying = false;
		}
		else if (type == (u_short)ePacketType::GameStart)
		{
			gameStart = true;
			input = 0;
		}
		else if (type == (u_short)ePacketType::GameEnd)
		{
			gameStart = false;
			elapsed = 0.0f;
			input = 0;
		}
		else if (type == (u_short)ePacketType::TimeOut)
		{
			u_short choice = input - '0';
			tPacketData data = WritePacket<u_short>(ePacketType::TimeOut, choice);
			send(hSocket, data.body, data.size, 0);
			elapsed = 0.0f;
			gamePlaying = false;
		}
		temp += sizeof(u_short);
	}

	return -1;
}

void Input()
{
	if (_kbhit())
	{
		char ch = _getch();

		if (ch == 27) // esc키
		{
			u_short temp = 0;
			tPacketData data = WritePacket<u_short>(ePacketType::PlayerDisconnect, temp);
			send(hSocket, data.body, data.size, 0);
		}
		else
		{
			if ('1' <= ch && ch <= '3')
				input = ch;
		}

		PrintBoard();
	}
}

void Update()
{
	while (!recvList.empty())
	{
		delete[] recvList.front();
		recvList.pop();
	}

}
