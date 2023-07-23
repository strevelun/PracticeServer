#include "Client.h"

#pragma comment( lib, "ws2_32.lib")

int main()
{
	Client client;

	if (client.Init("192.168.219.167", 30001))
		client.Run();

	client.Cleanup();

	return 0;
}