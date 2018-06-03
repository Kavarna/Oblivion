#pragma once


#include "../../common/common.h"
#include "../Helpers/Lights.h"
#include "../interfaces/Shader.h"



class TextureLightShader : public IShader
{
public:
	TextureLightShader();
	~TextureLightShader();

private:
	static std::once_flag						m_shaderFlags;
	static std::unique_ptr<TextureLightShader>	m_shaderInstance;

public:
	struct SCameraInfo
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

public:
	void Create()					override;
	void bind() const				override;
	
	void SetLightInformations(Sun const& sun);
	void SetCameraInformations(SCameraInfo const& camInfo);
public:
	static TextureLightShader* Get();

private:
	MicrosoftPointer<ID3D11VertexShader>		m_vertexShader;
	MicrosoftPointer<ID3D11PixelShader>			m_pixelShader;
	MicrosoftPointer<ID3D11InputLayout>			m_layout;
	std::array<MicrosoftPointer<ID3DBlob>,2>	m_shaderBlobs;
	
	MicrosoftPointer<ID3D11Buffer>				m_cameraBuffer; // dynamic buffer
	MicrosoftPointer<ID3D11Buffer>				m_lightBuffer; // default buffer
};
