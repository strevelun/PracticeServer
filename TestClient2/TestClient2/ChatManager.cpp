#include <iostream>
#include <conio.h>

#include "ChatManager.h"
#include "Settings.h"

ChatManager* ChatManager::m_inst = nullptr;

ChatManager::ChatManager()
{
}

ChatManager::~ChatManager()
{
	std::list<const char*>::iterator iter = m_chatList.begin();
	std::list<const char*>::iterator iterEnd = m_chatList.end();

	for (; iter != iterEnd; iter++)
		delete[] * iter;
}

ePacketType ChatManager::Input(bool& _isTyped)
{
	if (_kbhit())
	{
		char ch = _getch();
		_isTyped = true;

		if (ch == '\r' || ch == '\n') {
			if (m_inputBufferLoc <= 0) return ePacketType::None;
			m_inputBuffer[m_inputBufferLoc] = 0;
			m_inputBufferLoc = 0;
			return ePacketType::Chat;
		}
		else if (ch == '\b')
		{
			if (m_inputBufferLoc <= 0) return ePacketType::None;
			m_inputBuffer[--m_inputBufferLoc] = 0;
		}
		else if (ch == 27)
		{
			return ePacketType::Exit;
		}
		else
		{
			m_inputBuffer[m_inputBufferLoc++] = ch;
			m_inputBuffer[m_inputBufferLoc] = 0;
		}
	}
	return ePacketType::None;
}

void ChatManager::AddConversation(const char* _who, const char* _saidThat)
{
	const int conversationSize = 300;
	char* conversation = new char[conversationSize];
	sprintf_s(conversation, conversationSize, "[%s] : %s", _who, _saidThat);

	if (m_chatList.size() >= 10)
	{
		delete[] m_chatList.front();
		m_chatList.pop_front();
	}
	m_chatList.push_back(conversation);
}
