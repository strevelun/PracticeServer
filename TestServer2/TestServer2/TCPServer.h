#pragma once

#include "PacketQueue.h"
#include "PacketHandler.h"

#include <WinSock2.h>

class TCPServer
{
private:
	PacketQueue		m_packetQueue;

public:
	TCPServer();
	~TCPServer();

	bool Init(const char* _serverIP, int _serverPort);	
	void Update();
	void Cleanup();

	void ReceivePacket(SOCKET _clientSocket);
};

