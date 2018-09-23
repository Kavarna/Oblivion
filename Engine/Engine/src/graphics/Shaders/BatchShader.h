#pragma once


#include "../interfaces/Shader.h"
#include "../../common/interfaces/Singletone.h"

class BatchShader :
	public IShader, public Singletone<BatchShader>
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

public:
	void SetCamera(DirectX::FXMMATRIX&);

public:
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_objectBuffer;
};

class TextureBatchShader : public IShader, public Singletone<TextureBatchShader>
{
public:
	TextureBatchShader();
	~TextureBatchShader();

public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 texCoord;
	};

public:
	void Create() override;
	const Pipeline GetPipelineType() const override;


public:
	void SetCamera(DirectX::FXMMATRIX&);

public:
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_objectBuffer;
};
