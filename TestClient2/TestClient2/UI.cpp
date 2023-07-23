#include "UI.h"
#include "Chat.h"

UI::UI()
{
}

UI::~UI()
{
	std::list<const char*>::iterator iter = m_conversationList.begin();
	std::list<const char*>::iterator iterEnd = m_conversationList.end();

	for (; iter != iterEnd; iter++)
		delete[] * iter;
}

void UI::PrintBoard(const Chat& _chat)
{
	system("cls");
	for (int i = 0; i < 10 - m_conversationList.size(); i++) puts("");
	std::list<const char*>::iterator iter = m_conversationList.begin();
	std::list<const char*>::iterator iterEnd = m_conversationList.end();
	for (; iter != iterEnd; iter++)
		printf("%s\n", *iter);
	puts("=========================================================");
	printf("ют╥б : %s", _chat.GetInputBuffer());
}

void UI::AddConversation(const char* _conv)
{
	if (m_conversationList.size() >= 10)
	{
		delete[] m_conversationList.front();
		m_conversationList.pop_front();
	}
	m_conversationList.push_back(_conv);
}
