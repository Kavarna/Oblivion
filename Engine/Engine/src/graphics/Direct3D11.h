#pragma once


#include "../common/common.h"
#include "interfaces/Shader.h"
#include "interfaces/GameObject.h"
#include "interfaces/Camera.h"



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
	void					OnResize(HWND hWnd, uint32_t width, uint32_t height);

	ID3D11Device*			getDevice()		const { return m_d3d11Device.Get(); };
	ID3D11DeviceContext*	getContext()	const { return m_d3d11Context.Get(); };
	void					resetSwapChain() { m_dxgiSwapChain.Reset(); m_dxgiSwapChain = nullptr; };

	void					Begin();
	void					End();

public:
	void					createShader(IShader *);
	void					bindShader(IShader const*, IShader::SCameraInformations const* = nullptr, IShader::SMaterialInformations const * = nullptr);
	template <class shader>
	void					createGameObject(IGameObject<shader>*, IShader*);
	template <class shader>
	void					renderGameObject(ICamera*, IGameObject<shader>*, int instanceCount = 1);

private:
	void CreateDepthStencilView(uint32_t width, uint32_t height);

public:
	inline bool Available() const { return m_available; };

private:
	void CheckCommonShaderParts(IShader const* shader);

public:
	bool										m_hasMSAA					= false;
	bool										m_hasVerticalSync			= true;

private: // Attributes
	bool										m_available					= false;
	UINT										m_maxMSAAQualityLevel		= 0;

private: // On pipeline
	uint64_t									m_shaderCode				= { 0 };

private:
	D3D11_VIEWPORT								m_viewPort;
private:
	MicrosoftPointer<ID3D11Device>				m_d3d11Device				= nullptr;
	MicrosoftPointer<ID3D11DeviceContext>		m_d3d11Context				= nullptr;
	MicrosoftPointer<ID3D11RenderTargetView>	m_defaultRenderTarget		= nullptr;
	MicrosoftPointer<ID3D11DepthStencilView>	m_depthStencilView			= nullptr;
	MicrosoftPointer<IDXGISwapChain>			m_dxgiSwapChain				= nullptr;
	
};

template<class shaderType>
inline void Direct3D11::createGameObject(IGameObject<shaderType>* object, IShader * shader)
{
	object->Create((shaderType*)(shader), m_d3d11Device.Get(), m_d3d11Context.Get());
}

template<class shader>
inline void Direct3D11::renderGameObject(ICamera * camera, IGameObject<shader>* object, int instanceCount)
{
	IShader::SCameraInformations cam;
	cam.World = DirectX::XMMatrixTranspose(object->GetObjectWorld());
	cam.View = DirectX::XMMatrixTranspose(camera->GetView());
	cam.Projection = DirectX::XMMatrixTranspose(camera->GetProjection());
	IShader::SMaterialInformations mat;
	bindShader(object->m_shader, &cam, &mat);
	object->Render(m_d3d11Context.Get());
	if (instanceCount == 1)
		m_d3d11Context->DrawIndexed(object->GetIndexCount(),
			object->GetStartIndexLocation(), object->GetStartVertexLocation());
	else
		m_d3d11Context->DrawIndexedInstanced(object->GetIndexCount(), instanceCount,
			object->GetStartIndexLocation(), object->GetStartVertexLocation(), 0);
}
