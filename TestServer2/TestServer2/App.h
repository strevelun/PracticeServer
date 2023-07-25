#pragma once
class App
{
private:
	static App* m_inst;

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
};

