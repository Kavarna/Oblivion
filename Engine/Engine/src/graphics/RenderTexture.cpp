#include "RenderTexture.h"



RenderTexture::RenderTexture(uint32_t width, uint32_t height,
	RenderTextureFlags flags, uint32_t MSAACount, uint32_t MSAAQuality)
{
	Reset(width, height, flags, MSAACount, MSAAQuality);
}

RenderTexture::~RenderTexture()
{
}

void RenderTexture::Reset(uint32_t width, uint32_t height,
	RenderTextureFlags flags, uint32_t MSAACount, uint32_t MSAAQuality)
{
	using namespace TextureUtilities;
	Reset();
	UINT bindFlags = 0;
	m_textureFlags = flags;
	bool depth = false;
	if (flags & RenderTextureFlags::ColorReadEnable)
		bindFlags |= D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	if (flags & RenderTextureFlags::ColorWriteEnable)
		bindFlags |= D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
	if (flags & RenderTextureFlags::DepthWriteEnable)
		depth = true;
	if (flags & RenderTextureFlags::UnorderedAccess)
		bindFlags |= D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;

	if (bindFlags)
		CreateTexture(m_d3d11Device.Get(), &m_texture,
			width, height, bindFlags,
			DXGI_FORMAT_R32G32B32A32_FLOAT, MSAACount, MSAAQuality);

	if (flags & RenderTextureFlags::ColorWriteEnable)
		CreateRTVFromTexture(m_d3d11Device.Get(), m_texture.Get(), &m_renderTargetView);
	if (flags & RenderTextureFlags::ColorReadEnable)
		CreateSRVFromTexture(m_d3d11Device.Get(), m_texture.Get(), &m_shaderResourceView);
	if (flags & RenderTextureFlags::UnorderedAccess)
		CreateUAVFromTexture(m_d3d11Device.Get(), m_texture.Get(), &m_unorderedAccessView);

	if (depth)
	{
		bindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (flags & RenderTextureFlags::DepthReadEnable)
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		CreateTexture(m_d3d11Device.Get(), &m_textureDepth,
			width, height, bindFlags,
			DXGI_FORMAT_R32_TYPELESS, MSAACount, MSAAQuality);
		CreateDSVFromTexture(m_d3d11Device.Get(), m_textureDepth.Get(), &m_depthView,
			DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT);
		if (flags & RenderTextureFlags::DepthReadEnable)
		{
			CreateSRVFromTexture(m_d3d11Device.Get(), m_textureDepth.Get(), &m_depthShaderResource,
				DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);
			m_oblDepth = std::make_shared<Texture>(m_textureDepth.Get(), m_depthShaderResource.Get());
		}
	}

	if (m_texture)
		m_oblTexture = std::make_shared<Texture>(m_texture.Get(), m_shaderResourceView.Get(), m_unorderedAccessView.Get());

	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = (FLOAT)width;
	m_viewPort.Height = (FLOAT)height;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;
}

void RenderTexture::Clear(float r, float g, float b, float a)
{
	ClearTexture(r, g, b, a);
	ClearDepth();
}

void RenderTexture::ClearTexture(float r, float g, float b, float a)
{
	if (m_textureFlags & RenderTextureFlags::ColorWriteEnable)
	{
		FLOAT color[4] = { r,g,b,a };
		m_d3d11Context->ClearRenderTargetView(m_renderTargetView.Get(), color);
	}
}

void RenderTexture::ClearDepth()
{
	if (m_textureFlags & RenderTextureFlags::DepthWriteEnable)
	{
		m_d3d11Context->ClearDepthStencilView(m_depthView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
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
