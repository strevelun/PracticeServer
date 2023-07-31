#include "App.h"
#include "UIManager.h"
#include "UI.h"
#include "ChatManager.h"

#include <cstdio>
#include <Windows.h>

App* App::m_inst = nullptr;

App::App() 
{
}

App::~App()
{
	m_client.Cleanup();
	UIManager::GetInst()->DestroyInst();
}

void App::SetNickname(char* _nickname)
{
	memcpy(m_userName, _nickname, strlen(_nickname));
}

bool App::Init()
{
	WSADATA  wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed WSAStartup() \n");
		return false;
	}

	if (!UIManager::GetInst()->Init())
		return false;

	if (!m_client.Init("192.168.219.167", 30001))
		return false;

	return true;
}

void App::Run()
{
	UIManager* pUIManager = UIManager::GetInst();
	UI* pUI = pUIManager->GetUI(); // 잘못된 구조 UIManager::Update()
	ChatManager* chatManager = ChatManager::GetInst();

	char userName[UserNameLen];
	do {
		pUI->PrintGetNickName();
		scanf_s("%s", userName, UserNameLen);
	} while (strlen(userName) < 1);

	SetNickname(userName);

	pUI->PrintBoard(chatManager->GetChatList());

	while (m_isRunning)
	{
		ePacketType type = chatManager->Input();

		m_client.SendPacketByType(type);

		if (pUIManager->GetUpdateNeeded())
		{
			pUI->PrintBoard(chatManager->GetChatList());
			pUIManager->SetUpdateNeeded(false);
		}
	}
}