#include "PacketHandler.h"

#include <WinSock2.h>
#include <stdio.h>

PacketHandler::PacketHandler()
{
}

PacketHandler::~PacketHandler()
{
}

ePacketType PacketHandler::ReadPacket(char* _packet)
{
	char userName[UserNameLen]; ZeroMemory(userName, UserNameLen);
	char body[BodyLen]; ZeroMemory(body, BodyLen);
	int loc = sizeof(u_short);
	u_short type = *(u_short*)(_packet + loc);
	loc += sizeof(u_short);

	if ((ePacketType)type == ePacketType::Chat)
	{
		u_short userNameSize = *(u_short*)(_packet + loc);
		loc += sizeof(u_short);
		memcpy(userName, _packet + loc, userNameSize);
		loc += userNameSize;
		u_short bodySize = *(u_short*)(_packet + loc);
		loc += sizeof(u_short);
		memcpy(body, _packet + loc, bodySize);
		printf("[%s]°¡ º¸³¿ : %s\n", userName, body);
	}
	else if ((ePacketType)type == ePacketType::Exit)
	{
		printf("");
	}

	return (ePacketType)type;
}
