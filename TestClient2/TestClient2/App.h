#pragma once

#include "TCPClient.h"

class App
{
private:
	static App* m_inst;

	TCPClient m_client;
	char m_userName[UserNameLen] = {0};

	bool m_isRunning = true;

	App();
	~App();

public:
	static App* GetInst()
	{
		if (!m_inst)
			m_inst = new App;
		return m_inst;
	}

	static void DestroyInst()
	{
		if (m_inst)
			delete m_inst;
		m_inst = nullptr;
	}

	bool Init();
	void Run();
	TCPClient& FindMyClient() { return m_client; }
	const char* GetNickname() const { return m_userName; }

private:
	void SetNickname(char* _nickname);
};

