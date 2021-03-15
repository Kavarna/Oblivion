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
	void Create(HWND hWnd);
	void Run();
	void Destroy();

public:
	inline float GetNearZ() const { return 1.0f; };
	inline float GetFarZ() const { return 10000.0f; };

public:
	void OnSize(uint32_t width, uint32_t height);
	void OnMouseMove();

private:
	void InitWindow();
	void InitInput();
	void InitDirect3D();
	void InitImGui();
	void InitPipelines();
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
	void Display3();
	void Display4();
	void Display5();
	void Display6();
	void Display7();
	void Display8();
	void Display9();
	void Display10();


private:
	HINSTANCE											m_windowInstance = NULL;
	HWND												m_windowHandle = NULL;
	
	uint32_t											m_windowWidth = 0;
	uint32_t											m_windowHeight = 0;
	bool												m_menuActive = true;
	float												m_mouseSensivity = 2.0f;
	bool												m_showDeveloperConsole = false;
	bool												m_selectObjects = false;

private:
	/*std::unique_ptr<CollisionObject>					m_ground;
	std::unique_ptr<CollisionObject>					m_sphere;
	std::unique_ptr<CollisionObject>					m_tree;
	std::unique_ptr<CollisionObject>					m_cup;
	//std::unique_ptr<CollisionObject>					m_sponza;*/
	//std::unique_ptr<BillboardObject>					m_billboardTest;

	std::unique_ptr<ShadowmapBuild>						m_shadowMap;

	std::vector<IGameObject*>							m_gameObjects;
	std::vector<Model*>									m_models;
	Model*												m_selectedObject;

	std::unique_ptr<DirectionalLightView>				m_directionalLight;

	std::unique_ptr<BatchRenderer>                      m_batchRenderer;

	unsigned int                                        m_selectedDisplay = 0;

private:
	std::unique_ptr<DirectX::Mouse>						m_mouse;
	std::unique_ptr<DirectX::Keyboard>					m_keyboard;
};

