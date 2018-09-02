#pragma once


#include "interfaces/Object.h"
#include "Helpers/TextureUtilities.h"


class RenderTexture :
	public IObject
{
public:
	static void LuaRegister();
public:
	RenderTexture() = default;
	RenderTexture(uint32_t width, uint32_t height, bool depth = false, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0);
	~RenderTexture();

public:
	void Reset(uint32_t width, uint32_t height, bool depth = false, uint32_t MSAACount = 1, uint32_t MSAAQuality = 0);
	void ClearTexture(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
	void SetRenderTarget();

public:
	ID3D11Texture2D * GetTexture() { return m_texture.Get(); };
	ID3D11ShaderResourceView * GetTextureSRV() { return m_shaderResourceView.Get(); };
	ID3D11UnorderedAccessView * GetTextureUAV() { return m_unorderedAccessView.Get(); };



private:
	void Reset();

private:
	MicrosoftPointer<ID3D11Texture2D>			m_texture					= nullptr;
	MicrosoftPointer<ID3D11RenderTargetView>	m_renderTargetView			= nullptr;
	MicrosoftPointer<ID3D11ShaderResourceView>	m_shaderResourceView		= nullptr;
	MicrosoftPointer<ID3D11UnorderedAccessView>	m_unorderedAccessView		= nullptr;
	
	MicrosoftPointer<ID3D11Texture2D>			m_textureDepth				= nullptr;
	MicrosoftPointer<ID3D11DepthStencilView>	m_depthView					= nullptr;

	D3D11_VIEWPORT								m_viewPort;
};

