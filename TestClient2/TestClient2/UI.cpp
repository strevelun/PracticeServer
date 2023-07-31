#include "UI.h"
#include "ChatManager.h"

#include <list>

UI::UI()
{
}

UI::~UI()
{

}

void UI::PrintGetNickName()
{
	system("cls");
	printf("�г��� �Է� : ");
}

void UI::PrintBoard(const std::list<const char*>& chatList)
{
	system("cls");
	for (int i = 0; i < 10 - chatList.size(); i++) puts("");
	std::list<const char*>::const_iterator iter = chatList.begin();
	std::list<const char*>::const_iterator iterEnd = chatList.end();
	for (; iter != iterEnd; iter++)
		printf("%s\n", *iter);
	puts("=========================================================");
	printf("�Է� : %s", ChatManager::GetInst()->GetInputBuffer());
}
