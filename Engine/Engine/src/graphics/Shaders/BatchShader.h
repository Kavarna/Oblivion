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
	void bind() const override;
	
	MAKE_SINGLETONE(BatchShader);

public:
	void SetCamera(DirectX::FXMMATRIX&);

public:
	MicrosoftPointer<ID3D11VertexShader>		m_vertexShader;
	MicrosoftPointer<ID3D11PixelShader>			m_pixelShader;
	MicrosoftPointer<ID3D11InputLayout>			m_layout;
	std::array<MicrosoftPointer<ID3DBlob>, 2>	m_shaderBlobs;

	MicrosoftPointer<ID3D11Buffer>				m_objectBuffer;
};

