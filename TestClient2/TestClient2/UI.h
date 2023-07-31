#pragma once

class ChatManager;
#include <list>

class UI
{
public:
	UI();
	~UI();

	void PrintGetNickName();
	void PrintBoard(const std::list<const char*>& chatList);
};

