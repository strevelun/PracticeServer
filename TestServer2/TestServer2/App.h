#pragma once

#include "TCPServer.h"

class App
{
private:
	static App* m_inst;

	TCPServer m_chatServer;

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

	void SetIsRunning(bool _isRunning) { m_isRunning = _isRunning; }
};

