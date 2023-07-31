#pragma once


#include "Connector.h"
#include "PacketHandler.h"

class TCPClient
{
private:
	HANDLE m_hThread;

	Connector		m_connector;
	PacketHandler m_packetHandler;

public:
	TCPClient();
	~TCPClient();

	bool Init(const char* _serverIP, int _serverPort);
	void SendPacketByType(ePacketType _packetType);
	void Cleanup();

	unsigned int ReceivePacket();

private:
	ePacketType ProcessPacket(char* _packet);
	static unsigned int __stdcall ThreadFunc(void* _pArgs);
};

