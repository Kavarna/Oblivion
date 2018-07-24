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
	const Pipeline GetPipelineType() const override;

	void SetCameraInformations(SCameraInfo const& info) const;

	MAKE_SINGLETONE(BasicShader);


private:
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_cameraBuffer;

};