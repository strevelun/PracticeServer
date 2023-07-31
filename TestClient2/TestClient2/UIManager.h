#pragma once

class UI;

class UIManager
{
private:
	static UIManager* m_inst;

	UIManager();
	~UIManager();

	UI* m_pUI;

	bool m_updateNeeded = false;

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
	void SetUpdateNeeded(bool _updateNeeded) { m_updateNeeded = _updateNeeded; }

	bool GetUpdateNeeded() const { return m_updateNeeded; }
};

