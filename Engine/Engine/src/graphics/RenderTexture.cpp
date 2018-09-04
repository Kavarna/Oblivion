#include "RenderTexture.h"



RenderTexture::RenderTexture(uint32_t width, uint32_t height,
	bool depth, uint32_t MSAACount, uint32_t MSAAQuality)
{
	Reset(width, height, depth, MSAACount, MSAAQuality);
}

RenderTexture::~RenderTexture()
{
}

void RenderTexture::Reset(uint32_t width, uint32_t height,
	bool depth, uint32_t MSAACount, uint32_t MSAAQuality)
{
	using namespace TextureUtilities;
	Reset();
	CreateTexture(m_d3d11Device.Get(), &m_texture,
		width, height, D3D11_BIND_RENDER_TARGET |
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
		DXGI_FORMAT_R32G32B32A32_FLOAT, MSAACount, MSAAQuality);

	CreateRTVFromTexture(m_d3d11Device.Get(), m_texture.Get(), &m_renderTargetView);
	CreateSRVFromTexture(m_d3d11Device.Get(), m_texture.Get(), &m_shaderResourceView);
	CreateUAVFromTexture(m_d3d11Device.Get(), m_texture.Get(), &m_unorderedAccessView);

	if (depth)
	{
		CreateTexture(m_d3d11Device.Get(), &m_textureDepth,
			width, height, D3D11_BIND_DEPTH_STENCIL,
			DXGI_FORMAT_D32_FLOAT, MSAACount, MSAAQuality);
		CreateDSVFromTexture(m_d3d11Device.Get(), m_textureDepth.Get(), &m_depthView);
	}

	m_oblTexture = std::make_shared<Texture>(m_texture.Get(), m_shaderResourceView.Get(), m_unorderedAccessView.Get());

	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = (FLOAT)width;
	m_viewPort.Height = (FLOAT)height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;
}

void RenderTexture::ClearTexture(float r, float g, float b, float a)
{
	FLOAT color[4] = { r,g,b,a };
	m_d3d11Context->ClearRenderTargetView(m_renderTargetView.Get(), color);
}

void RenderTexture::SetRenderTarget()
{
	// m_depthView might be null, but that's not a problem
	m_d3d11Context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthView.Get());
	m_d3d11Context->RSSetViewports(1, &m_viewPort);
}

void RenderTexture::Reset()
{
	m_texture.Reset();
	m_renderTargetView.Reset();
	m_shaderResourceView.Reset();
	m_unorderedAccessView.Reset();

	m_textureDepth.Reset();
	m_depthView.Reset();
}
