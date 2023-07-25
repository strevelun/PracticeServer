#include "App.h"
#include "UIManager.h"

#include <cstdio>

App* App::m_inst = nullptr;

App::App()
{
}

App::~App()
{
	m_client.Cleanup();
	UIManager::GetInst()->DestroyInst();
}

bool App::Init()
{
	if (!UIManager::GetInst()->Init())
		return false;

	if (!m_client.Init("192.168.219.167", 30001))
		return false;

	return true;
}

void App::Run()
{
	UI* pUI = UIManager::GetInst()->GetUI();

	char userName[UserNameLen];
	do {
		pUI->PrintGetNickName();
		scanf_s("%s", userName, UserNameLen);
	} while (strlen(userName) < 1);

	m_client.SetNickname(userName);

	pUI->PrintBoard();

	while (m_isRunning)
	{
		m_client.Update();
	}
}