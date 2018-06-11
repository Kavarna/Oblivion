#pragma once

#include "OblivionInclude.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "graphics/Shaders/BasicShader.h"
#include "graphics/Shaders/TextureLightShader.h"
#include "graphics/Camera.h"
#include "graphics/Model.h"

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
	void Init3D();

	void InitSizeDependent();

	void Begin();
	void End();

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
private:
	std::unique_ptr<Camera>						m_camera;
	std::unique_ptr<Model>						m_sphereModel;
	std::unique_ptr<Model>						m_groundModel;
private:
	std::unique_ptr<DirectX::Mouse>				m_mouse;
	std::unique_ptr<DirectX::Keyboard>			m_keyboard;
};

