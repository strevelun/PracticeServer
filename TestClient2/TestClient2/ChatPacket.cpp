#include "ChatPacket.h"
#include "Settings.h"

ChatPacket::ChatPacket(const char* _userName, int _userNameSize, const char* _message, int _messageSize)
	: m_userName(_userName), m_message(_message),
	m_userNameSize(_userNameSize), m_messageSize(_messageSize),
	Packet(sizeof(u_short) + _userNameSize + sizeof(u_short) + _messageSize, (u_short)ePacketType::Chat)
{
	char* tempBuffer = m_packetBuffer;
	*(u_short*)tempBuffer = m_size;						tempBuffer += sizeof(u_short);
	*(u_short*)tempBuffer = m_type;						tempBuffer += sizeof(u_short);
	*(u_short*)tempBuffer = (u_short)m_userNameSize;	tempBuffer += sizeof(u_short);
	memcpy(tempBuffer, m_userName, m_userNameSize);		tempBuffer += m_userNameSize;
	*(u_short*)tempBuffer = (u_short)m_messageSize;		tempBuffer += sizeof(u_short);
	memcpy(tempBuffer, m_message, m_messageSize);		tempBuffer += m_messageSize;
}

ChatPacket::~ChatPacket()
{
}
