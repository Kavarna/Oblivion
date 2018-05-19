#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow)
{

	try
	{
		Game* joc = Game::GetInstance();
		joc->Create(hInstance);
		joc->Run();
		joc->Destroy();
	}
	catch (std::exception const& e)
	{
		wchar_t buffer[500];
		size_t converted = 0;
		mbstowcs_s(&converted, buffer, e.what(), 500);
		MessageBox(NULL, buffer, L"Error", MB_ICONERROR | MB_OK);
		return 1;
	}
	catch (...)
	{
		MessageBox(NULL, L"Unexpected error", L"Error", MB_ICONERROR | MB_OK);
	}
	return 0;
}