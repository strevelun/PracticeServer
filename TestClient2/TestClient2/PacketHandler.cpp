#include "PacketHandler.h"
#include "Connector.h"
#include "UI.h"
#include "Chat.h"

#include <winsock2.h>
#include <iostream>

PacketHandler::PacketHandler(Connector* _pConnector, UI* _pUI, Chat* _pChat)
	: m_pConnector(_pConnector), m_pUI(_pUI), m_pChat(_pChat)
{
}

PacketHandler::~PacketHandler()
{
}

ePacketType PacketHandler::ProcessPacket(char* _packet, int _packetSize)
{
	char* tempPacket = _packet;
	tempPacket += sizeof(u_short);
	u_short type = *(u_short*)tempPacket;
	tempPacket += sizeof(u_short);

	if ((ePacketType)type == ePacketType::Chat)
	{
		return ProcessChatPacket(tempPacket);
	}
	else if ((ePacketType)type == ePacketType::Exit)
	{
		return ePacketType::Exit;
	}
	return ePacketType::None;
}

ePacketType PacketHandler::ProcessChatPacket(char* _packet)
{
	char userName[UserNameLen];
	char body[BodyLen];

	u_short userNameSize = *(u_short*)_packet;				_packet += sizeof(u_short);
	memcpy(userName, _packet, userNameSize);				_packet += userNameSize;
	userName[userNameSize] = 0;
	u_short bodySize = *(u_short*)_packet;					_packet += sizeof(u_short);
	memcpy(body, _packet, bodySize);
	body[bodySize] = 0;

	const int conversationSize = 300;
	char* conversation = new char[conversationSize];
	sprintf_s(conversation, conversationSize, "[%s] : %s", userName, body);

	m_pUI->AddConversation(conversation);
	m_pUI->PrintBoard(*m_pChat);

	return ePacketType::Chat;
}
