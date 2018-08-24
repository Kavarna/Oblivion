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
	void RegisterEngine();
	void OpenScripts();

	void Begin();
	void End();

	void WriteSettings();
	
	bool PickObject();

private:
	//void AddEntityModel(Entity* entity, std::string const& path, int numInstances = 1);
	//void AddEntityCollisionObject(Entity* entity, std::string const& path, float mass, int numInstances = 1);
	void AddEntity(Entity* e, const Script* s, const std::string& tablename);

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

	std::vector<std::unique_ptr<GameScript>>			m_gameScripts;
	std::vector<std::unique_ptr<Entity>>				m_entities;
	std::map<std::string, std::unique_ptr<Model>>		m_models;
	Model*												m_selectedObject;

private:
	std::unique_ptr<DirectX::Mouse>						m_mouse;
	std::unique_ptr<DirectX::Keyboard>					m_keyboard;
};

