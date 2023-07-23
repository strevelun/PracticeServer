#pragma once

#include "Settings.h"

class Connector;
class UI;
class Chat;

class PacketHandler
{
private:
	Connector*		m_pConnector;
	UI*				m_pUI;
	Chat*			m_pChat;

public:
	PacketHandler(Connector* _pConnector, UI* _pUI, Chat* _pChat);
	~PacketHandler();

	ePacketType ProcessPacket(char* _packet, int _packetSize);

private:
	ePacketType ProcessChatPacket(char* _packet);
};

