#include "Packet.h"

Packet::Packet(u_short _size, u_short _type)
	: m_size(sizeof(u_short) + sizeof(u_short) + _size), m_type(_type)
{
}

Packet::~Packet()
{
}
