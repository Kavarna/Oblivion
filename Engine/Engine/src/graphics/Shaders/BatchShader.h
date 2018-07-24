#pragma once


#include "../interfaces/Shader.h"

class BatchShader :
	public IShader
{
public:
	BatchShader();
	~BatchShader();

public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

public:
	void Create() override;
	const Pipeline GetPipelineType() const override;
	
	MAKE_SINGLETONE(BatchShader);

public:
	void SetCamera(DirectX::FXMMATRIX&);

public:
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_objectBuffer;
};

