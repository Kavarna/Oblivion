#pragma once


#include "../../common/common.h"
#include "../Helpers/Lights.h"
#include "../interfaces/Shader.h"
#include "../interfaces/ICamera.h"


class DisplacementShader :
	public IShader
{
public:
	DisplacementShader();
	~DisplacementShader();

	MAKE_SINGLETONE(DisplacementShader);

public:
	void Create() override;
	const Pipeline GetPipelineType() const	override;

	void SetCameraInfo(ICamera * cam);

private:
	struct DSCameraBuffer
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};
	struct VSCameraBuffer
	{
		DirectX::XMFLOAT3 eyePosition;
		float minTessDistance;
		float maxTessDistance;
		DirectX::XMFLOAT3 pad;
	};

public:
	std::array<MicrosoftPointer<ID3DBlob>, 4>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_cameraBufferVS;
	MicrosoftPointer<ID3D11Buffer>				m_cameraBufferDS;
};

