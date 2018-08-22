#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow)
{

	try
	{
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(hr))
			throw std::exception("Can't initialize COM");
		COM::InitializeComObjects();
		LuaManager::InitializeLua();
		Game* joc = Game::GetInstance();
		joc->Create(hInstance);
		joc->Run();
		//joc->Destroy();
		COM::UninitializeComObjects();
		CoUninitialize();
	}
	catch (std::exception const& e)
	{
		wchar_t buffer[5000];
		size_t converted = 0;
		mbstowcs_s(&converted, buffer, e.what(), 5000);
		MessageBox(NULL, buffer, L"Error", MB_ICONERROR | MB_OK);
		CoUninitialize();
		return 1;
	}
	catch (...)
	{
		MessageBox(NULL, L"Unexpected error", L"Error", MB_ICONERROR | MB_OK);
		CoUninitialize();
		return 1;
	}
	return 0;
}