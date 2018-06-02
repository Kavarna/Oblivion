#pragma once


#include "interfaces/Shader.h"

class BasicShader : public IShader
{
public:
	BasicShader();
	~BasicShader();

public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
	};
	struct SCameraInfo
	{
		DirectX::XMMATRIX WVP;
	};
public:
	void Create();
	void bind() const;

	void SetCameraInformations(SCameraInfo const& info) const;
private:
	static std::once_flag						m_shaderFlags;
	static std::unique_ptr<BasicShader>			m_shaderInstance;

public:
	static BasicShader* Get();

private:
	MicrosoftPointer<ID3D11VertexShader>		m_vertexShader;
	MicrosoftPointer<ID3D11PixelShader>			m_pixelShader;
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;
	MicrosoftPointer<ID3D11InputLayout>			m_layout;

	MicrosoftPointer<ID3D11Buffer>				m_cameraBuffer;

};