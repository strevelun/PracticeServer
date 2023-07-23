#pragma once

#include <list>

class Chat;

class UI
{
private:
	std::list<const char*> m_conversationList;

public:
	UI();
	~UI();

	void PrintBoard(const Chat& _chat);
	void AddConversation(const char* _conv);
};

