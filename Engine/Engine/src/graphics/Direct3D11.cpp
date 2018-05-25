#include "Direct3D11.h"


std::once_flag					Direct3D11::m_d3d11Flag;
std::unique_ptr<Direct3D11>		Direct3D11::m_d3d11Instance = nullptr;


Direct3D11* Direct3D11::GetInstance()
{
	std::call_once(m_d3d11Flag, [&] { m_d3d11Instance = std::make_unique<Direct3D11>(); });

	return m_d3d11Instance.get();
}

Direct3D11::Direct3D11()
{

}

Direct3D11::~Direct3D11()
{
	m_d3d11Device.Reset();
	m_d3d11Context.Reset();
	m_defaultRenderTarget.Reset();
	m_dxgiSwapChain.Reset();
	m_depthStencilView.Reset();
}

void Direct3D11::Create(HWND window)
{	
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_1,
	};

	UINT flags = 0;
#ifndef USE_MULTITHREADED_D3D11
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif
#if DEBUG || _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL finalLevel;
	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		levels, _countof(levels), D3D11_SDK_VERSION, &m_d3d11Device, &finalLevel, &m_d3d11Context);
	if (FAILED(hr))
		throw std::exception("Can't create direct3d device");

	MicrosoftPointer<ID3D11Debug> debug;
	if (SUCCEEDED(m_d3d11Device.As(&debug)))
	{
		MicrosoftPointer<ID3D11InfoQueue> infoQueue;
		if (SUCCEEDED(debug.As(&infoQueue)))
		{
#if DEBUG || _DEBUG
			infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY::D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
#endif
			// TODO: Add disabled messages
		}
	}
}

void Direct3D11::OnResize(HWND hWnd, uint32_t width, uint32_t height)
{
	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width	= (FLOAT) width;
	m_viewPort.Height	= (FLOAT) height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;
	if (!m_d3d11Device)
		return;
	static DXGI_FORMAT swapChainFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	static uint16_t bufferCount = 2;
	static UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (m_dxgiSwapChain)
	{ // Resize it
		m_defaultRenderTarget.Reset();
		m_dxgiSwapChain->ResizeBuffers(bufferCount, width, height, swapChainFormat, swapChainFlags);
		MicrosoftPointer<ID3D11Texture2D> renderTargetResource;
		ThrowIfFailed(
			m_dxgiSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), &renderTargetResource)
			);
		ThrowIfFailed(
			m_d3d11Device->CreateRenderTargetView(renderTargetResource.Get(), nullptr, &m_defaultRenderTarget)
			);
		CreateDepthStencilView(width, height);
		// TODO: Add fullscreen
	}
	else
	{ // Create it
		MicrosoftPointer<IDXGIDevice> dxgiDevice;
		if (SUCCEEDED(m_d3d11Device.As(&dxgiDevice)))
		{
			MicrosoftPointer<IDXGIAdapter> adapter;
			if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
			{
				MicrosoftPointer<IDXGIFactory> finalFactory;
				adapter->GetParent(__uuidof( IDXGIFactory ), &finalFactory);

				DXGI_SWAP_CHAIN_DESC swapDesc = {};
				swapDesc.BufferCount = bufferCount;
				swapDesc.BufferDesc.Format = swapChainFormat;
				swapDesc.BufferDesc.Width = width;
				swapDesc.BufferDesc.Height = height;
				swapDesc.BufferDesc.RefreshRate.Denominator = 1;
				swapDesc.BufferDesc.RefreshRate.Numerator = 60;
				swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
				swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
				swapDesc.OutputWindow = hWnd;

				if (m_hasMSAA)
				{
					m_d3d11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_maxMSAAQualityLevel);
					if (m_maxMSAAQualityLevel > 0)
						swapDesc.SampleDesc.Quality = m_maxMSAAQualityLevel - 1;
					else
					{
						swapDesc.SampleDesc.Quality = 0;
						m_hasMSAA = false;
					}
					swapDesc.SampleDesc.Count = 4;
				}
				else
				{
					swapDesc.SampleDesc.Quality = 0;
					swapDesc.SampleDesc.Count = 1;
				}

				swapDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
				
				swapDesc.Windowed = true;

				ThrowIfFailed(
					finalFactory->CreateSwapChain(m_d3d11Device.Get(), &swapDesc, &m_dxgiSwapChain)
					);

				MicrosoftPointer<ID3D11Texture2D> renderTargetResource;
				ThrowIfFailed(
					m_dxgiSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), &renderTargetResource)
					);
				ThrowIfFailed(
					m_d3d11Device->CreateRenderTargetView(renderTargetResource.Get(), nullptr, &m_defaultRenderTarget)
					);
				m_available = true;
				CreateDepthStencilView(width, height);
			}
		}
	}
}

void Direct3D11::CreateDepthStencilView(uint32_t width, uint32_t height)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;

	if (m_hasMSAA)
	{
		if (m_maxMSAAQualityLevel > 0)
			texDesc.SampleDesc.Quality = m_maxMSAAQualityLevel - 1;
		else
		{
			texDesc.SampleDesc.Quality = 0;
			m_hasMSAA = false;
		}
		texDesc.SampleDesc.Count = 4;
	}
	else
	{
		texDesc.SampleDesc.Quality = 0;
		texDesc.SampleDesc.Count = 1;
	}

	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	MicrosoftPointer<ID3D11Texture2D> depthBuffer;
	ThrowIfFailed(
		m_d3d11Device->CreateTexture2D(&texDesc, nullptr, &depthBuffer)
		);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
	dsViewDesc.Format = texDesc.Format;
	dsViewDesc.Texture2D.MipSlice = 0;
	if (m_hasMSAA)
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DMS;
	else
		dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
	ThrowIfFailed(
		m_d3d11Device->CreateDepthStencilView(depthBuffer.Get(), &dsViewDesc, &m_depthStencilView)
		);
}

void Direct3D11::Begin()
{
	m_d3d11Context->OMSetRenderTargets(1, m_defaultRenderTarget.GetAddressOf(), nullptr);
	static FLOAT color[4] = { 0.0f,0.0f,0.0f,1.0f };
	m_d3d11Context->ClearRenderTargetView(m_defaultRenderTarget.Get(), color);
	m_d3d11Context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_d3d11Context->RSSetViewports(1, &m_viewPort);
}

void Direct3D11::End()
{
	if (m_hasVerticalSync)
		m_dxgiSwapChain->Present(1, 0);
	else
		m_dxgiSwapChain->Present(0, 0);
}

void Direct3D11::createShader(IShader * shader)
{
	shader->Create(m_d3d11Device.Get());
}

void Direct3D11::bindShader(IShader const* shader, IShader::SCameraInformations const* cam, IShader::SMaterialInformations const* mat)
{
#if DEBUG || _DEBUG
	CheckCommonShaderParts(shader);
#endif
	if (!shader->additionalData())
		shader->bind(m_d3d11Context.Get());
	else
	{
		shader->bindCameraInformations(m_d3d11Context.Get(), cam);
		shader->bindMaterialInformations(m_d3d11Context.Get(), mat);
		shader->bind(m_d3d11Context.Get());
	}
}

void Direct3D11::CheckCommonShaderParts(IShader const* shader)
{
	if (m_shaderCode == shader->m_shaderCode)
		DX::OutputVDebugString(L"[LOG]: Same shader being bind multiple times. This is a performance hit.\n");
	m_shaderCode = shader->m_shaderCode;
}
