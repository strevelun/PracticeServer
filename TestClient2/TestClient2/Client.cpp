#include "Client.h"
#include "Connector.h"
#include "ChatPacket.h"
#include "ExitPacket.h"
#include "UIManager.h"
#include "ChatManager.h"

#include <stdio.h>
#include <conio.h>
#include <thread>
#include <Windows.h>

using namespace std;

Client::Client()
{
}

Client::~Client()
{
	delete[] m_userName;
	CloseHandle(m_hThread);
}

bool Client::Init(const char* _serverIP, int _serverPort)
{
	WSADATA  wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
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

	unsigned int threadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &threadID);
	if (m_hThread == NULL)
	{
		printf("Failed BeginThreadEx\n");
		return false;
	}

    return true;
}

void Client::Cleanup()
{
	m_connector.Disconnect();
	WSACleanup();
}

void Client::SetNickname(char* _nickname)
{
	memcpy(m_userName, _nickname, strlen(_nickname));
}

unsigned int Client::ReceivePacket()
{
	int							recvSize, curTotalRecvSize = 0;
	unsigned short				packetSize;
	char						recvBuffer[255];
	int bufferSize = sizeof(recvBuffer);

	bool isRunning = true;

	while (isRunning)
	{
		recvSize = m_connector.Receive(recvBuffer + curTotalRecvSize, bufferSize - curTotalRecvSize);
		if (recvSize == -1) break;

		curTotalRecvSize += recvSize;

		while (1)
		{
			packetSize = *(u_short*)recvBuffer;
			if (curTotalRecvSize < 2 || packetSize > curTotalRecvSize) break;

			ePacketType type = ProcessPacket(recvBuffer);

			if (type == ePacketType::Exit)
			{
				isRunning = false;
				break;
			}

			curTotalRecvSize -= packetSize;
			memcpy(recvBuffer, recvBuffer + packetSize, curTotalRecvSize);
		}
	}
	return 0;
}

ePacketType Client::ProcessPacket(char* _packet)
{
	char* tempPacket = _packet;
	tempPacket += sizeof(u_short);
	u_short type = *(u_short*)tempPacket;

	if ((ePacketType)type == ePacketType::Chat)
	{
		return m_packetHandler.HandleChatPacket(tempPacket);
	}
	else if ((ePacketType)type == ePacketType::Exit)
	{
		return ePacketType::Exit;
	}
	return ePacketType::None;
}

unsigned int __stdcall Client::ThreadFunc(void* _pArgs)
{
	Client* pClient = static_cast<Client*>(_pArgs);
	return pClient->ReceivePacket();
}

void Client::Update()
{	
	UI* pUI = UIManager::GetInst()->GetUI();
	ChatManager* chatManager = ChatManager::GetInst();
	
	bool isTyped = false;
	ePacketType type = chatManager->Input(isTyped);

	if (isTyped == true)
		pUI->PrintBoard();

	if (type == ePacketType::Exit)
	{
		ExitPacket* exitPacket = new ExitPacket();
		m_connector.Send(exitPacket);
	}
	else if (type == ePacketType::Chat)
	{
		const char* inputBuffer = chatManager->GetInputBuffer();
		ChatPacket* chatPacket = new ChatPacket(m_userName, strlen(m_userName), inputBuffer, strlen(inputBuffer));
		m_connector.Send(chatPacket);
		chatManager->ClearInputBuffer();
	}
}
