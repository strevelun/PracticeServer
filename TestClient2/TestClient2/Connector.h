#pragma once

#include <WS2tcpip.h>

class Connector
{
private:
	SOCKADDR_IN  m_servAddr;
	SOCKET   m_hClientSocket;

public:
	Connector();
	~Connector();

	SOCKET GetClientSocket() const { return m_hClientSocket; }

	bool Init();
	int Receive(char* _buffer, int _bufferSize);
	int Send(class Packet* _packet);
	bool Connect(const char* _serverIP, int _serverPort);
	void Disconnect();
};

