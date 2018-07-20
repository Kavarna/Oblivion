#pragma once


#include "../common/common.h"



class Direct3D11
{
	template <class type>
	using Ptr = MicrosoftPointer<type>;
public:
	static Direct3D11* Get();

private:
	static std::once_flag						m_d3d11Flag;
	static std::unique_ptr<Direct3D11>			m_d3d11Instance;

public:
	Direct3D11();
	~Direct3D11();

public:
	void										Create(HWND window);
	void										OnResize(HWND hWnd, uint32_t width, uint32_t height);

	Ptr<ID3D11Device>							getDevice()		const { return m_d3d11Device; };
	Ptr<ID3D11DeviceContext>					getContext()	const { return m_d3d11Context; };
	void										resetSwapChain() { m_dxgiSwapChain.Reset(); m_dxgiSwapChain = nullptr; };

	void										Begin();
	void										End();
	void										RSWireframeRender();
	void										RSSolidRender();
	void										RSCullNone();
	void										DepthEnableLess();
	void										DepthDisable();
	void										OMTransparency(float blendFactor);
	void										OMDefault();

	void										DefaultStates();

private:
	void										CreateDepthStencilView(uint32_t width, uint32_t height);
	void										InitializeStates();
public:
	inline bool									Available() const { return m_available; };

public:
	bool										m_hasMSAA					= false;
	bool										m_hasVerticalSync			= true;

private: // Attributes
	bool										m_available					= false;
	UINT										m_maxMSAAQualityLevel		= 0;

public: // States
	// Rasterizer
	MicrosoftPointer<ID3D11RasterizerState>		m_wireFrameNoCulling		= nullptr;
	MicrosoftPointer<ID3D11RasterizerState>		m_solidBackfaceCulling		= nullptr;
	MicrosoftPointer<ID3D11RasterizerState>		m_solidNoCulling			= nullptr;
	// Samplers
	MicrosoftPointer<ID3D11SamplerState>		m_linearWrapSamplerState	= nullptr;
	// Blend
	MicrosoftPointer<ID3D11BlendState>			m_transparencyBlendState	= nullptr;
	// Depth
	MicrosoftPointer<ID3D11DepthStencilState>	m_depthEnabledLess			= nullptr;
	MicrosoftPointer<ID3D11DepthStencilState>	m_depthDisabled				= nullptr;

private:
	D3D11_VIEWPORT								m_viewPort;
private:
	MicrosoftPointer<ID3D11Device>				m_d3d11Device				= nullptr;
	MicrosoftPointer<ID3D11DeviceContext>		m_d3d11Context				= nullptr;
	MicrosoftPointer<ID3D11RenderTargetView>	m_defaultRenderTarget		= nullptr;
	MicrosoftPointer<ID3D11DepthStencilView>	m_depthStencilView			= nullptr;
	MicrosoftPointer<IDXGISwapChain>			m_dxgiSwapChain				= nullptr;
	
};