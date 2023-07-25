#pragma once

#include <winsock2.h>

class Packet
{
protected:
	char m_packetBuffer[255];

	u_short m_size;
	u_short m_type;

public:
	Packet(u_short _size, u_short _type);
	~Packet();

	u_short GetPacketSize() const { return m_size; }
	const char* GetPacketBuffer() const { return m_packetBuffer; }
};

