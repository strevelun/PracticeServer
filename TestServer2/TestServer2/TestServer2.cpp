#include "App.h"

#pragma comment( lib, "ws2_32.lib")

int main()
{
	App* pApp = App::GetInst();

	if (pApp->Init())
		pApp->Run();

	pApp->DestroyInst();

	return 0;
}