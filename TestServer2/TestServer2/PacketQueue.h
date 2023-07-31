#pragma once

#include "PacketHandler.h"

#include <list>

class PacketQueue
{
private:
	PacketHandler	m_packetHandler;

	std::list<char*> m_packetList;

public:
	PacketQueue();
	~PacketQueue();

	void AddPacket(char* _packet);
	void ProcessPacketList();
};

