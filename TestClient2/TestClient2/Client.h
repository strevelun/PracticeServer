#pragma once


#include "Connector.h"
#include "UI.h"
#include "PacketHandler.h"

class Client
{
private:
	HANDLE m_hThread;

	Connector		m_connector;
	PacketHandler m_packetHandler;

	char m_userName[UserNameLen];

public:
	Client();
	~Client();

	bool Init(const char* _serverIP, int _serverPort);
	void Update();
	void Cleanup();

	void SetNickname(char* _nickname);

	unsigned int ReceivePacket();

private:
	ePacketType ProcessPacket(char* _packet);
	static unsigned int __stdcall ThreadFunc(void* _pArgs);
};

