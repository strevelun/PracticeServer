#include "Listener.h"

#include <stdio.h>
#include <WS2tcpip.h>

Listener::Listener()
{
}

Listener::~Listener()
{
}

bool Listener::Init()
{
	m_hSocketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_hSocketServer == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return false;
	}

    return true;
}

SOCKET Listener::Accept(SOCKADDR_IN* _addrClient, int* _addrSize)
{
	return accept(m_hSocketServer, (SOCKADDR*)_addrClient, _addrSize);
}

int Listener::Receive(SOCKET _socket, char* _buffer, int _bufferSize)
{
    return recv(_socket, _buffer, _bufferSize, 0);
}

int Listener::Send(SOCKET _socket, Packet* _packet)
{
    return 0;
}

bool Listener::Connect(const char* _serverIP, int _serverPort)
{
	memset(&m_servAddr, 0, sizeof(m_servAddr));
	m_servAddr.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, _serverIP, &m_servAddr.sin_addr);
	m_servAddr.sin_port = htons(_serverPort);

	if (bind(m_hSocketServer, (SOCKADDR*)&m_servAddr, sizeof(m_servAddr)) == SOCKET_ERROR)
	{
		printf("Binding Error \n");
		return false;
	}
	if (listen(m_hSocketServer, 3) == SOCKET_ERROR)
	{
		printf("listen Error \n");
		return false;
	}

    return true;
}

void Listener::Disconnect()
{
	if (m_hSocketServer != INVALID_SOCKET) closesocket(m_hSocketServer);
}
