#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow)
{

	Game* joc = Game::GetInstance();
	joc->Create(hInstance);
	joc->Run();
	return 0;
}