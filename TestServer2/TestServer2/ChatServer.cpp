#include "ChatServer.h"
#include "Settings.h"

#include <stdio.h>

ChatServer::ChatServer()
{
}

ChatServer::~ChatServer()
{
}

bool ChatServer::Init(const char* _serverIP, int _serverPort)
{
	WSADATA  wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed WSAStartup() \n");
		return false;
	}

	if (!m_listener.Init())
	{
		printf("Failed Listener::Init() \n");
		return false;
	}

	if (!m_listener.Connect(_serverIP, _serverPort))
	{
		printf("Failed Listener::Connect() \n");
		return false;
	}

    return true;
}

void ChatServer::Run()
{
	fd_set				fdSocketInfos, fdReads;
	u_int				i;
	SOCKET				clientSocket;
	int					iRet;

	SOCKET hSocketServer = m_listener.GetSocketServer();

	FD_ZERO(&fdSocketInfos);
	FD_SET(hSocketServer, &fdSocketInfos);

	while (true)
	{
		fdReads = fdSocketInfos;
		iRet = select(0, &fdReads, 0, 0, 0);    // accept 와 recv 두개 감시
		if (iRet == SOCKET_ERROR) break;

		for (i = 0; i < fdSocketInfos.fd_count; i++)
		{
			clientSocket = fdSocketInfos.fd_array[i];
			if (FD_ISSET(clientSocket, &fdReads))
			{
				if (clientSocket == hSocketServer)
				{
					AcceptClient(fdSocketInfos);
				}
				else
				{
					ReceiveAndReadPacket(fdSocketInfos, clientSocket);
				}
			}
		}
	}
}

void ChatServer::Cleanup()
{
	m_listener.Disconnect();
	WSACleanup();
}

void ChatServer::AcceptClient(fd_set& _pfdSocketInfos)
{
	SOCKADDR_IN			addrClient;
	int addrSize = sizeof(addrClient);
	SOCKET clientSocket = m_listener.Accept(&addrClient, &addrSize);
	printf("%d 연결됨\n", (int)clientSocket);
	FD_SET(clientSocket, &_pfdSocketInfos);
}

void ChatServer::ReceiveAndReadPacket(fd_set& _pfdSocketInfos, SOCKET _clientSocket)
{
	int recvSize, curTotalRecvSize = 0;
	SOCKET socketTemp2;
	u_short		packetSize;
	char				recvBuffer[255];

	recvSize = m_listener.Receive(_clientSocket, recvBuffer, sizeof(recvBuffer));
	if (recvSize > 0)
	{
		curTotalRecvSize += recvSize;

		while (1)
		{
			packetSize = *(u_short*)recvBuffer;
			if (curTotalRecvSize < 2 || packetSize > curTotalRecvSize) break;

			ePacketType type = m_packetHandler.ReadPacket(recvBuffer);
			if (type == ePacketType::Chat)
			{
				for (int j = 1; j < _pfdSocketInfos.fd_count; j++)
				{
					socketTemp2 = _pfdSocketInfos.fd_array[j];
					send(socketTemp2, recvBuffer, packetSize, 0);
				}
			}

			curTotalRecvSize -= packetSize;
			memcpy(recvBuffer, recvBuffer + packetSize, curTotalRecvSize);
		}
	}
	else
	{
		FD_CLR(_clientSocket, &_pfdSocketInfos);
		closesocket(_clientSocket);
	}
}
