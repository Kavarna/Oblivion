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
	void Create() override;
	const Pipeline GetPipelineType() const override;

	void SetCameraInformations(SCameraInfo const& camInfo);
	void SetAdditionalColor(const DirectX::XMFLOAT4& color);

private:
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_cameraBuffer; // dynamic buffer
	MicrosoftPointer<ID3D11Buffer>				m_additionalColorBuffer;
};

