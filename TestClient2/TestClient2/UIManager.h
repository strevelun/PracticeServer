#pragma once
class UIManager
{
private:
	static UIManager* m_inst;

	UIManager();
	~UIManager();

	class UI* m_pUI;

public:
	static UIManager* GetInst()
	{
		if (!m_inst) m_inst = new UIManager();
		return m_inst;
	}

	static void DestroyInst()
	{
		if (m_inst) delete m_inst;
		m_inst = nullptr;
	}

	bool Init();
	UI* GetUI() { return m_pUI; }
};

