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
	Destroy();
}

void Game::Destroy()
{
	m_basicModel.reset();
	m_basicShader.reset();

	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

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
	InitImGui();
	InitShaders();
	Init3D();
	InitSizeDependent();
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

	m_windowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, ENGINE_NAME, ENGINE_NAME,
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
	m_mouse->SetVisible(true);
	m_mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
}

void Game::InitDirect3D()
{
	Direct3D11 * d3d = Direct3D11::GetInstance();
	d3d->Create(m_windowHandle);
	d3d->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);
}

void Game::InitImGui()
{
	Direct3D11 * d3d = Direct3D11::GetInstance();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplDX11_Init(m_windowHandle, d3d->getDevice(), d3d->getContext());

	ImGui::StyleColorsClassic();
}

void Game::InitShaders()
{
	auto renderer = Direct3D11::GetInstance();
	m_basicShader = std::make_unique<BasicShader>();
	renderer->createShader(m_basicShader.get());
}

void Game::Init3D()
{
	auto renderer = Direct3D11::GetInstance();
	m_basicModel = std::make_unique<Model<BasicShader>>();
	renderer->createGameObject(m_basicModel.get(), m_basicShader.get());
}

void Game::InitSizeDependent()
{
	//float FOV, float HByW, float NearZ, float FarZ
	float FOV = DirectX::XM_PI / 5;
	float HByW = (float)m_windowHeight / (float)m_windowWidth;
	float nearZ = 1.0f;
	float farZ = 1000.0f;
	m_camera = std::make_unique<Camera>(FOV, HByW, nearZ, farZ);
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
		else
		{
			Update();
			Render();
		}
	}
}

void Game::Update()
{
	float frameTime = 1000.0f / ImGui::GetIO().Framerate;
	auto kb = m_keyboard->GetState();
	auto mouse = m_mouse->GetState();
	static bool bEscape = false;
	if (kb.Escape && !bEscape)
	{
		bEscape = true;
		if (m_menuActive)
			m_mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
		else
			m_mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
		m_menuActive = !m_menuActive;
	}
	else if (!kb.Escape)
		bEscape = false;
	float cameraFrameTime = frameTime;
	if (kb.LeftShift)
		cameraFrameTime *= 10;
	if (kb.W)
		m_camera->WalkForward(cameraFrameTime);
	if (kb.S)
		m_camera->WalkBackward(cameraFrameTime);
	if (kb.D)
		m_camera->StrafeRight(cameraFrameTime);
	if (kb.A)
		m_camera->StrafeLeft(cameraFrameTime);

	m_camera->Update(frameTime, 0.0f, 0.0f);
}

void Game::Begin()
{
	auto renderer = Direct3D11::GetInstance();
	renderer->Begin();
	ImGui_ImplDX11_NewFrame();
}

void Game::End()
{
	auto mouse = m_mouse->GetState();
	auto renderer = Direct3D11::GetInstance();

#if DEBUG || _DEBUG
	ImGui::Begin("Debug");
	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
#endif

	bool hasMSAA = renderer->m_hasMSAA;

	ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoNav);
	ImGui::Checkbox("Vertical sync", &renderer->m_hasVerticalSync);
	ImGui::Checkbox("Use 4xMSAA", &renderer->m_hasMSAA);

	if (hasMSAA != renderer->m_hasMSAA)
	{
		renderer->resetSwapChain();
		renderer->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	renderer->End();
}

void Game::Render()
{
	auto renderer = Direct3D11::GetInstance();
	if (!renderer->Available())
		return;
	Begin();

	renderer->renderGameObject(m_camera.get(), m_basicModel.get());

	End();
}

void Game::OnSize(uint32_t width, uint32_t height)
{
	if (width < 10 || height < 10)
		return;
	m_windowWidth = width;
	m_windowHeight = height;

	InitSizeDependent();

	Direct3D11::GetInstance()->OnResize(m_windowHandle, m_windowWidth, m_windowHeight);

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

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