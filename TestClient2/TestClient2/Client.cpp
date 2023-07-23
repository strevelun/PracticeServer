#include "Client.h"
#include "Connector.h"
#include "ChatPacket.h"
#include "ExitPacket.h"

#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <thread>

using namespace std;

Client::Client() 
	: m_isRunning(true), 
	m_pPacketHandler(nullptr),
	m_hThread(INVALID_HANDLE_VALUE)
{
}

Client::~Client()
{
	delete[] m_userName;
	delete m_pPacketHandler;

	CloseHandle(m_hThread);
}

bool Client::Init(const char* _serverIP, int _serverPort)
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		printf("Failed WSAStartup() \n");
		return false;
	}

	if (!m_connector.Init())
	{
		printf("Failed Connector::Init() \n");
		return false;
	}

	if (!m_connector.Connect(_serverIP, _serverPort))
	{
		printf("Failed Connector::Connect() \n");
		return false;
	}

	m_pPacketHandler = new PacketHandler(&m_connector, &m_ui, &m_chat);

	unsigned int threadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &threadID);

    return true;
}

void Client::Cleanup()
{
	m_connector.Disconnect();
	WSACleanup();
}

void Client::Run()
{	
	do {
		system("cls");
		printf("닉네임 입력 : ");
		scanf_s("%s", m_userName, UserNameLen);
	} while (strlen(m_userName) < 1);
	
	m_ui.PrintBoard(m_chat);

	while (1)
	{
		if (!m_isRunning) break;

		bool isTyped = false;
		ePacketType type = m_chat.Input(isTyped);

		if (isTyped == true)
			m_ui.PrintBoard(m_chat);

		if (type == ePacketType::Exit)
		{
			ExitPacket* exitPacket = new ExitPacket();
			m_connector.Send(exitPacket);
		}
		else if (type == ePacketType::Chat)
		{
			const char* inputBuffer = m_chat.GetInputBuffer();
			ChatPacket* chatPacket = new ChatPacket(m_userName, strlen(m_userName), inputBuffer, strlen(inputBuffer));
			m_connector.Send(chatPacket);
			m_chat.ClearInputBuffer();
		}
	}
}

unsigned int Client::ReceiveAndProcessPacket()
{
	int					recvSize, curTotalRecvSize = 0;
	u_short				packetSize;
	char				recvBuffer[255];
	int bufferSize = sizeof(recvBuffer);

	while (m_isRunning)
	{
		recvSize = m_connector.Receive(recvBuffer + curTotalRecvSize, bufferSize - curTotalRecvSize);
		if (recvSize == -1) break;

		curTotalRecvSize += recvSize;

		while (1)
		{
			packetSize = *(u_short*)recvBuffer;
			if (curTotalRecvSize < 2 || packetSize > curTotalRecvSize) break;

			ePacketType type = m_pPacketHandler->ProcessPacket(recvBuffer, packetSize);
			if (type == ePacketType::Exit)
			{
				m_isRunning = false;
				break;
			}

			curTotalRecvSize -= packetSize;
			memcpy(recvBuffer, recvBuffer + packetSize, curTotalRecvSize);
		}
	}
	return 0;
}

unsigned int __stdcall Client::ThreadFunc(void* _pArgs)
{
	Client* client = static_cast<Client*>(_pArgs);
	return client->ReceiveAndProcessPacket();
}
