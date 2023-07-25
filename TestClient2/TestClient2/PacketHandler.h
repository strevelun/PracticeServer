#pragma once

#include "Settings.h"

class Connector;
class UI;
class ChatManager;

class PacketHandler
{

public:
	PacketHandler();
	~PacketHandler();

public:
	ePacketType HandleChatPacket(char* _packet);
};

