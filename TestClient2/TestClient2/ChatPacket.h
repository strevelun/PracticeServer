#pragma once
#include "Packet.h"
class ChatPacket :
    public Packet
{
private:
	const char*			m_userName;
	int					m_userNameSize;
	const char*			m_message;
	int					m_messageSize;

public:
	ChatPacket(const char* _userName, int _userNameSize, const char* _message, int _messageSize);
	~ChatPacket();

	char* MakePacket() override;
};

