#include "PacketHandler.h"
#include "Selector.h"

#include <WinSock2.h>
#include <stdio.h>

PacketHandler::PacketHandler()
{
}

PacketHandler::~PacketHandler()
{
}

ePacketType PacketHandler::HandleChatPacket(char* _packet, int _packetSize)
{
	char* temp = _packet;
	u_short type = *(u_short*)temp;
	if (type != (u_short)ePacketType::Chat) return ePacketType::None;
	temp += sizeof(u_short);

	char userName[UserNameLen];
	char body[BodyLen];

	u_short userNameSize = *(u_short*)temp;				temp += sizeof(u_short);
	memcpy(userName, temp, userNameSize);				temp += userNameSize;
	userName[userNameSize] = 0;
	u_short bodySize = *(u_short*)temp;					temp += sizeof(u_short);
	memcpy(body, temp, bodySize);
	body[bodySize] = 0;

	printf("[%s]°¡ º¸³¿ : %s\n", userName, body);

	fd_set infos = Selector::GetInst()->GetSocketInfos();
	SOCKET clientSocket;

	for (u_int i = 1; i < infos.fd_count; i++)
	{
		clientSocket = infos.fd_array[i];
		send(clientSocket, _packet - sizeof(u_short), _packetSize, 0);
	}

	return (ePacketType)type;
}

ePacketType PacketHandler::HandleExitPacket(char* _packet)
{
	u_short type = *(u_short*)_packet;
	if (type != (u_short)ePacketType::Exit) return ePacketType::None;
	_packet += sizeof(u_short);

	// sendall

	return (ePacketType)type;
}
