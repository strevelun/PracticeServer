#include "ChatServer.h"

#pragma comment( lib, "ws2_32.lib")

int main()
{
	ChatServer server;
	if (server.Init("192.168.219.167", 30001))
		server.Run();

	server.Cleanup();

	return 0;
}