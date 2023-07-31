#include "PacketHandler.h"
#include "Connector.h"
#include "UI.h"
#include "ChatManager.h"
#include "UIManager.h"

#include <winsock2.h>
#include <iostream>

PacketHandler::PacketHandler()
{
}

PacketHandler::~PacketHandler()
{
}


ePacketType PacketHandler::HandleChatPacket(char* _packet)
{
	u_short type = *(u_short*)_packet;
	if (type != (u_short)ePacketType::Chat) return ePacketType::None;
	_packet += sizeof(u_short);

	UI* pUI = UIManager::GetInst()->GetUI();

	char userName[UserNameLen];
	char body[BodyLen];

	u_short userNameSize = *(u_short*)_packet;				_packet += sizeof(u_short);
	memcpy(userName, _packet, userNameSize);				_packet += userNameSize;
	userName[userNameSize] = 0;
	u_short bodySize = *(u_short*)_packet;					_packet += sizeof(u_short);
	memcpy(body, _packet, bodySize);
	body[bodySize] = 0;

	ChatManager::GetInst()->AddChat(userName, body);
	UIManager::GetInst()->SetUpdateNeeded(true);

	return ePacketType::Chat;
}
