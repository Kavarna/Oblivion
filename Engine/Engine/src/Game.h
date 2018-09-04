#pragma once

#include "OblivionInclude.h"

struct SAnimation
{
	void Update(float frameTime);
	void OnMouseMove();

private:
	BezierEasing easing = BezierEasing({ 0.61f, 0.07f }, { 0.46f, 0.89f });
	bool bActive = false;
	float accumulatedTime;
	const float animationScale = 0.1f;

	DirectX::XMFLOAT2 currentPosition = DirectX::XMFLOAT2(0.0f, 0.0f);
	DirectX::XMFLOAT2 startPosition = DirectX::XMFLOAT2(0.0f, 0.0f);
	DirectX::XMFLOAT2 endPosition = DirectX::XMFLOAT2(0.0f, 0.0f);

	DirectX::XMFLOAT2 dir;

	DirectX::XMFLOAT2 lastMouseCoords;
	DirectX::XMFLOAT2 currMouseCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Game sealed
{
	friend struct SAnimation;
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
	void OnMouseMove();

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
	HINSTANCE											m_windowInstance;
	HWND												m_windowHandle;
	
	uint32_t											m_windowWidth;
	uint32_t											m_windowHeight;
	bool												m_menuActive = true;
	float												m_mouseSensivity = 2.0f;
	bool												m_showDeveloperConsole = false;
	bool												m_selectObjects = false;

private:
#if DEBUG || _DEBUG
	std::unique_ptr<Square>								m_debugSquare;
#endif
	std::unique_ptr<Square>								m_animationSquare;
	//std::unique_ptr<CollisionObject>					m_ground;
	//std::unique_ptr<CollisionObject>					m_sphere;
	//std::unique_ptr<CollisionObject>					m_tree;

	//std::unique_ptr<Texture>							m_level;

	std::vector<Model*>									m_models;
	Model*												m_selectedObject;


	SAnimation											m_animation;

private:
	std::unique_ptr<DirectX::Mouse>						m_mouse;
	std::unique_ptr<DirectX::Keyboard>					m_keyboard;
};

