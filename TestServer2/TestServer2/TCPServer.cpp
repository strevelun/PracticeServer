#include "TCPServer.h"
#include "Settings.h"
#include "Listener.h"
#include "Selector.h"

#include <stdio.h>


TCPServer::TCPServer()
{
}

TCPServer::~TCPServer()
{
}

bool TCPServer::Init(const char* _serverIP, int _serverPort)
{
	WSADATA  wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed WSAStartup() \n");
		return false;
	}

	Listener* pListener = Listener::GetInst();

	if (!pListener->Init(_serverIP, _serverPort)) return false;

	if (!Selector::GetInst()->Init(pListener->GetSocketServer())) return false;

    return true;
}

void TCPServer::Update()
{
	Selector::GetInst()->Select();
	std::list<SOCKET>& clientSocketList = Selector::GetInst()->GetClientSocketList();
	while (clientSocketList.size() > 0)
	{
		ReceivePacket(clientSocketList.front());
		clientSocketList.pop_front();
	}

	m_packetQueue.ProcessPacketList();
}

void TCPServer::Cleanup()
{
	Listener::GetInst()->Disconnect();
	WSACleanup();
}

void TCPServer::ReceivePacket(SOCKET _clientSocket)
{
	fd_set& pfdSocketInfos = Selector::GetInst()->GetSocketInfos();
	int					recvSize, curTotalRecvSize = 0;
	SOCKET				socketTemp2 = 0;
	u_short				packetSize = 0;
	char				recvBuffer[255];

	recvSize = recv(_clientSocket, recvBuffer, sizeof(recvBuffer), 0);
	if (recvSize > 0)
	{
		curTotalRecvSize += recvSize;

		while (1)
		{
			packetSize = *(u_short*)recvBuffer;
			if (curTotalRecvSize < 2 || packetSize > curTotalRecvSize) break;

			char* packet = new char[packetSize];
			memcpy(packet, recvBuffer, packetSize);
			m_packetQueue.AddPacket(packet);

			curTotalRecvSize -= packetSize;
			memcpy(recvBuffer, recvBuffer + packetSize, curTotalRecvSize);
		}
	}
	else
	{
		FD_CLR(_clientSocket, &pfdSocketInfos);
		closesocket(_clientSocket);
	}
}
