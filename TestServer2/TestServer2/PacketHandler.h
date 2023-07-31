#pragma once

#include "Settings.h"

class PacketHandler
{
private:

public:
	PacketHandler();
	~PacketHandler();

	ePacketType HandleChatPacket(char* _packet, int _packetSize);
	ePacketType HandleExitPacket(char* _packet);
};

