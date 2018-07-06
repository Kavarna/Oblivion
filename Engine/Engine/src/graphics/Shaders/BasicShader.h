#pragma once


#include "../interfaces/Shader.h"

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
	void Create() override;
	void bind() const override;

	void SetCameraInformations(SCameraInfo const& info) const;

	MAKE_SINGLETONE(BasicShader);


private:
	MicrosoftPointer<ID3D11VertexShader>		m_vertexShader;
	MicrosoftPointer<ID3D11PixelShader>			m_pixelShader;
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;
	MicrosoftPointer<ID3D11InputLayout>			m_layout;

	MicrosoftPointer<ID3D11Buffer>				m_cameraBuffer;

};