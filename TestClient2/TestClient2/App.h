#pragma once

#include "Client.h"

class App
{
private:
	static App* m_inst;

	Client m_client;

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
	Client& FindMyClient() { return m_client; }
};

