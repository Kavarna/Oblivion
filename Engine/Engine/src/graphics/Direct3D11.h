#pragma once


#include "../common/common.h"


class Direct3D11
{
public:
	static Direct3D11* GetInstance();

private:
	static std::once_flag						m_d3d11Flag;
	static std::unique_ptr<Direct3D11>			m_d3d11Instance;

public:
	Direct3D11();
	~Direct3D11();

public:
	void					Create(HWND window);
	void					OnResize(HWND hWnd, uint32_t width, uint32_t height, bool fullscreen);

	ID3D11Device*			getDevice()		const { return m_d3d11Device.Get(); };
	ID3D11DeviceContext*	getContext()	const { return m_d3d11Context.Get(); };

	void					Begin();
	void					End();

private:
	void CreateDepthStencilView(uint32_t width, uint32_t height);

public:
	inline bool Available() const { return m_available; };

public:
	bool										m_hasMSAA = false; // TODO: Add
	bool										m_hasVerticalSync = false;

private: // Attributes
	bool										m_available					= false;
	UINT										m_maxMSAAQualityLevel		= 0;

private:
	D3D11_VIEWPORT								m_viewPort;
private:
	MicrosoftPointer<ID3D11Device>				m_d3d11Device				= nullptr;
	MicrosoftPointer<ID3D11DeviceContext>		m_d3d11Context				= nullptr;
	MicrosoftPointer<ID3D11RenderTargetView>	m_defaultRenderTarget		= nullptr;
	MicrosoftPointer<ID3D11DepthStencilView>	m_depthStencilView			= nullptr;
	MicrosoftPointer<IDXGISwapChain>			m_dxgiSwapChain				= nullptr;
	
};