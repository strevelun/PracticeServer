#pragma once

#include <WinSock2.h>

class Listener
{
private:
	SOCKADDR_IN		m_servAddr;
	SOCKET			m_hSocketServer;

public:
	Listener();
	~Listener();

	bool Init();
	SOCKET Accept(SOCKADDR_IN* _addrClient, int* _addrSize);
	int Receive(SOCKET _socket, char* _buffer, int _bufferSize);
	int Send(SOCKET _socket, class Packet* _packet);
	bool Connect(const char* _serverIP, int _serverPort);
	void Disconnect();


	SOCKET GetSocketServer() const { return m_hSocketServer; }
};

