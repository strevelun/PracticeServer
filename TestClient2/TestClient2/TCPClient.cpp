#include "TCPClient.h"
#include "Connector.h"
#include "ChatPacket.h"
#include "ExitPacket.h"
#include "UIManager.h"
#include "ChatManager.h"
#include "App.h"

#include <stdio.h>
#include <conio.h>
#include <thread>

using namespace std;

TCPClient::TCPClient()
{
}

TCPClient::~TCPClient()
{
	CloseHandle(m_hThread);
}

bool TCPClient::Init(const char* _serverIP, int _serverPort)
{


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

void TCPClient::Cleanup()
{
	m_connector.Disconnect();
	WSACleanup();
}

unsigned int TCPClient::ReceivePacket()
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

ePacketType TCPClient::ProcessPacket(char* _packet)
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

unsigned int __stdcall TCPClient::ThreadFunc(void* _pArgs)
{
	TCPClient* pClient = static_cast<TCPClient*>(_pArgs);
	return pClient->ReceivePacket();
}

void TCPClient::SendPacketByType(ePacketType _packetType)
{		
	if (_packetType == ePacketType::Exit)
	{
		ExitPacket* exitPacket = new ExitPacket();
		m_connector.Send(exitPacket);
		delete exitPacket;
	}
	else if (_packetType == ePacketType::Chat)
	{
		ChatManager* chatManager = ChatManager::GetInst();
		const char* nickname = App::GetInst()->GetNickname();
		const char* inputBuffer = chatManager->GetInputBuffer();
		ChatPacket* chatPacket = new ChatPacket(nickname, strlen(nickname), inputBuffer, strlen(inputBuffer));
		m_connector.Send(chatPacket);
		chatManager->ClearInputBuffer();
		delete chatPacket;
	}
}
