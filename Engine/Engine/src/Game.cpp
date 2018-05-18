#include "Game.h"

std::once_flag			Game::m_gameFlag;
std::unique_ptr<Game>	Game::m_gameInstance = nullptr;


Game* Game::GetInstance()
{
	std::call_once(m_gameFlag, [&] { m_gameInstance = std::make_unique<Game>(); });

	return m_gameInstance.get();
}


Game::Game()
{}


Game::~Game()
{
	UnregisterClass(ENGINE_NAME, m_windowInstance);
	DestroyWindow(m_windowHandle);
}


void Game::Create(HINSTANCE hInstance, uint32_t width, uint32_t height)
{
	m_windowInstance = hInstance;
	m_windowWidth = width;
	m_windowHeight = height;
	
	InitWindow();
	InitInput();
	InitDirect3D();
}

void Game::InitWindow()
{
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = NULL;
	wndClass.hIcon = NULL; // TODO: To be added
	wndClass.hIconSm = NULL;
	wndClass.hInstance = m_windowInstance;
	wndClass.lpszClassName = ENGINE_NAME;
	wndClass.lpfnWndProc = WndProc;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wndClass);

	m_windowHandle = CreateWindow(ENGINE_NAME, ENGINE_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		m_windowWidth, m_windowHeight, nullptr, nullptr,
		m_windowInstance, nullptr);

	UpdateWindow(m_windowHandle);
	ShowWindow(m_windowHandle, SW_SHOWNORMAL);
	SetFocus(m_windowHandle);
}

void Game::InitInput()
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(m_windowHandle);
	m_mouse->SetVisible(false);
}

void Game::InitDirect3D()
{
	Direct3D11 * d3d = Direct3D11::GetInstance();
	d3d->Create(m_windowHandle);
	d3d->OnResize(m_windowHandle, m_windowWidth, m_windowHeight, false);
}

void Game::Run()
{
	MSG message;
	while (true)
	{
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				break;
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		Update();
		Render();
	}
}

void Game::Update()
{
	auto kb = m_keyboard->GetState();
	if (kb.Escape)
		PostQuitMessage(0);
}

void Game::Render()
{
	auto renderer = Direct3D11::GetInstance();
	if (!renderer->Available())
		return;
	renderer->Begin();

	renderer->End();
}

void Game::OnSize(uint32_t width, uint32_t height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	Direct3D11::GetInstance()->OnResize(m_windowHandle, m_windowWidth, m_windowHeight, false);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_ACTIVATEAPP:
	DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
	DirectX::Mouse::ProcessMessage(Message, wParam, lParam);
	break;

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
	DirectX::Mouse::ProcessMessage(Message, wParam, lParam);
	break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
	break;

	case WM_SIZE:
	Game::GetInstance()->OnSize(LOWORD(lParam), HIWORD(lParam));
	break;

	case WM_QUIT:
	DestroyWindow(hWnd);
	break;
	case WM_DESTROY:
	PostQuitMessage(0);
	break;
	}
	return DefWindowProc(hWnd, Message, wParam, lParam);
}