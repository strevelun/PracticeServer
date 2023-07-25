#pragma once

#include "Listener.h"
#include "PacketHandler.h"

#include <winsock2.h>

class ChatServer
{
private:
	Listener m_listener;
	PacketHandler m_packetHandler;

public:
	ChatServer();
	~ChatServer();

	bool Init(const char* _serverIP, int _serverPort);	
	void Run();
	void Cleanup();

	void AcceptClient(fd_set& _pfdSocketInfos);
	void ReceiveAndReadPacket(fd_set& _pfdSocketInfos, SOCKET _clientSocket);
};

