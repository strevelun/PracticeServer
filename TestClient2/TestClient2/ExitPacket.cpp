#include "ExitPacket.h"
#include "Settings.h"

ExitPacket::ExitPacket()
	: Packet(0, (u_short)ePacketType::Exit)
{
}

ExitPacket::~ExitPacket()
{
}

char* ExitPacket::MakePacket()
{
	char* tempBuffer = m_packetBuffer;
	*(u_short*)tempBuffer = m_size;				tempBuffer += sizeof(u_short);
	*(u_short*)tempBuffer = m_type;
	return m_packetBuffer;
}
