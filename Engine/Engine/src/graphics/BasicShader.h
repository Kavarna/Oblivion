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

protected:
	void Create(ID3D11Device*);
	void bind(ID3D11DeviceContext*) const;

private:
	MicrosoftPointer<ID3D11VertexShader> m_vertexShader;
	MicrosoftPointer<ID3D11PixelShader> m_pixelShader;
	std::array<MicrosoftPointer<ID3DBlob>, 2> m_shaderBlobs;
	MicrosoftPointer<ID3D11InputLayout> m_layout;

};