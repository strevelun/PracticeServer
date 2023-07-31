#include "PacketQueue.h"

PacketQueue::PacketQueue()
{
}

PacketQueue::~PacketQueue()
{
}

void PacketQueue::AddPacket(char* _packet)
{
	m_packetList.push_back(_packet);
}

void PacketQueue::ProcessPacketList()
{
	while (!m_packetList.empty())
	{
		char* packet = m_packetList.front();
		char* temp = packet;
		temp += sizeof(unsigned short);
		unsigned short type = *(unsigned short*)temp;

		if ((ePacketType)type == ePacketType::Chat)
			m_packetHandler.HandleChatPacket(temp, *(unsigned short*)packet);
		else if ((ePacketType)type == ePacketType::Exit)
			m_packetHandler.HandleExitPacket(temp);

		m_packetList.pop_front();
	}
}
