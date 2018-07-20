#pragma once

#include "../../common/common.h"
#include "../Helpers/Lights.h"
#include "../interfaces/Shader.h"

class TextureShader : public IShader
{
public:
	TextureShader();
	~TextureShader();

	MAKE_SINGLETONE(TextureShader);

public:
	struct SCameraInfo
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

public:
	void Create()					override;
	void bind() const				override;

	void SetCameraInformations(SCameraInfo const& camInfo);

private:
	MicrosoftPointer<ID3D11VertexShader>		m_vertexShader;
	MicrosoftPointer<ID3D11PixelShader>			m_pixelShader;
	MicrosoftPointer<ID3D11InputLayout>			m_layout;
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_cameraBuffer; // dynamic buffer
};

