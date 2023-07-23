#pragma once

#include <winsock2.h>
#include <list>

#include "Connector.h"
#include "Chat.h"
#include "UI.h"
#include "PacketHandler.h"

class Client
{
private:

	Connector		m_connector;
	Chat			m_chat;
	UI				m_ui;
	PacketHandler*	m_pPacketHandler;

	HANDLE m_hThread;

	bool m_isRunning = true;

	WSADATA  m_wsaData;

	char m_userName[UserNameLen];

public:
	Client();
	~Client();

	bool Init(const char* _serverIP, int _serverPort);
	void Run();
	void Cleanup();

private:
	unsigned int ReceiveAndProcessPacket();
	static unsigned int __stdcall ThreadFunc(void* _pArgs);
};

