#include "App.h"

#pragma comment( lib, "ws2_32.lib")

int main()
{
	if (App::GetInst()->Init())
		App::GetInst()->Run();

	App::GetInst()->DestroyInst();

	return 0;
}