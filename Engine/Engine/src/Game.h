#pragma once

#include "OblivionInclude.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Game sealed
{
	friend LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
public:
	static Game* GetInstance();

private:
	static std::once_flag						m_gameFlag;
	static std::unique_ptr<Game>				m_gameInstance;

public:
	Game();
	~Game();

public:
	void Create(HINSTANCE hInstance, uint32_t width = 800, uint32_t height = 600);
	void Run();
	void Destroy();

public:
	void OnSize(uint32_t width, uint32_t height);

private:
	void InitWindow();
	void InitInput();
	void InitDirect3D();
	void InitImGui();
	void Init2D();
	void Init3D();
	void InitSizeDependent();
	void InitSettings();

	void Begin();
	void End();

	void WriteSettings();
	
	bool PickObject();
private:
	void Update();
	void Render();

private:
	HINSTANCE									m_windowInstance;
	HWND										m_windowHandle;
	
	uint32_t									m_windowWidth;
	uint32_t									m_windowHeight;
	bool										m_menuActive = true;
	float										m_mouseSensivity = 2.0f;
	bool										m_showDeveloperConsole = false;
	bool										m_selectObjects = false;

private:
	std::unique_ptr<Camera>						m_camera;
	std::unique_ptr<Projection>					m_screen;
#if DEBUG || _DEBUG
	std::unique_ptr<Square>						m_debugSquare;
#endif
	std::unique_ptr<Model>						m_groundModel;
	std::unique_ptr<Model>						m_treeModel;
	std::unique_ptr<Model>						m_offRoadCar;

	std::vector<Model*>							m_models;
	Model*										m_selectedObject;
private:
	std::unique_ptr<DirectX::Mouse>				m_mouse;
	std::unique_ptr<DirectX::Keyboard>			m_keyboard;
};

