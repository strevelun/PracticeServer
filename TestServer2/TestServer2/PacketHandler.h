#pragma once

#include "Settings.h"

class PacketHandler
{
private:

public:
	PacketHandler();
	~PacketHandler();

	ePacketType ReadPacket(char* _packet);
};

