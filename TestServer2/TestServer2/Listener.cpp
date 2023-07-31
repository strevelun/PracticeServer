#include "Listener.h"

#include <stdio.h>
#include <WS2tcpip.h>

Listener* Listener::m_inst = nullptr;

Listener::Listener()
{
}

Listener::~Listener()
{
}

bool Listener::Init(const char* _serverIP, int _serverPort)
{
	m_hSocketServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_hSocketServer == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return false;
	}

	Connect(_serverIP, _serverPort);

    return true;
}

SOCKET Listener::AcceptClient()
{
	SOCKADDR_IN			addrClient;
	int addrSize = sizeof(addrClient);
	return accept(m_hSocketServer, (SOCKADDR*)&addrClient, &addrSize);
}

bool Listener::Connect(const char* _serverIP, int _serverPort, int _backlog)
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
	if (listen(m_hSocketServer, _backlog) == SOCKET_ERROR)
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

void Listener::Listen()
{
}
