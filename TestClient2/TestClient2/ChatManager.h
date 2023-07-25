#pragma once

#include <list>

#include "Settings.h"

class ChatManager
{
private:
	static ChatManager* m_inst;

	char m_inputBuffer[BodyLen];
	int m_inputBufferLoc = 0;

	std::list<const char*> m_chatList;

	ChatManager();
	~ChatManager();

public:
	static ChatManager* GetInst()
	{
		if (!m_inst) m_inst = new ChatManager;
		return m_inst;
	}

	static void DestroyInst()
	{
		if (m_inst) delete m_inst;
		m_inst = nullptr;
	}

	const char* GetInputBuffer() const { return m_inputBuffer; }

	ePacketType Input(bool& _isTyped);

	void ClearInputBuffer() { m_inputBuffer[0] = 0; }
	void AddConversation(const char* _who, const char* _saidThat);

	const std::list<const char*>& GetChatList() const { return m_chatList; };
};

