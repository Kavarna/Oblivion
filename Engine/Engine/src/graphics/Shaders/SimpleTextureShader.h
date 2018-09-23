#pragma once

#include "../../common/common.h"
#include "../../common/interfaces/Singletone.h"
#include "../Helpers/Lights.h"
#include "../interfaces/Shader.h"



/// Used for 2d rendering without instances (e.g. text)
class SimpleTextureShader : public IShader, public Singletone<SimpleTextureShader>
{
public:
	SimpleTextureShader();
	~SimpleTextureShader();

public:
	void Create() override;
	const Pipeline GetPipelineType() const override;

	void SetCameraInfo(DirectX::FXMMATRIX& WVP);
	void SetColor(const DirectX::XMFLOAT4 color);

private:
	MicrosoftPointer<ID3D11Buffer>					m_cameraBuffer;
	MicrosoftPointer<ID3D11Buffer>					m_colorBuffer;

	std::array<MicrosoftPointer<ID3DBlob>, 2>		m_shaderBlobs;
};