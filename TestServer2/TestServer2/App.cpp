#include "App.h"
#include "TCPServer.h"

App* App::m_inst = nullptr;

App::App()
{
}

App::~App()
{
}

bool App::Init()
{
	if(!m_chatServer.Init("192.168.219.167", 30001)) return false;

	return true;
}

void App::Run()
{
	while (m_isRunning)
	{
		m_chatServer.Update();
	}
}
