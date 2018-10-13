#include "SimpleVertexShader.h"

SimpleVertexShader::SimpleVertexShader() :
	IVertexShader(L"Shaders/SimpleVertexShader.cso")
{
}

void SimpleVertexShader::Create()
{
	createInputLayout();
}

void SimpleVertexShader::createInputLayout()
{
	LPVOID shaderPointer	= getShaderBlobPointer();
	SIZE_T shaderSize		= getShaderBlobSize();

	D3D11_INPUT_ELEMENT_DESC layout[5];
	layout[0].AlignedByteOffset = 0;
	layout[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;
	layout[0].SemanticIndex = 0;
	layout[0].SemanticName = "POSITION";

	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;
	layout[1].SemanticIndex = 0;
	layout[1].SemanticName = "TEXCOORD";

	layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[2].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;
	layout[2].SemanticIndex = 0;
	layout[2].SemanticName = "NORMAL";

	layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[3].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[3].InputSlot = 0;
	layout[3].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[3].InstanceDataStepRate = 0;
	layout[3].SemanticIndex = 0;
	layout[3].SemanticName = "TANGENT";

	layout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[4].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[4].InputSlot = 0;
	layout[4].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[4].InstanceDataStepRate = 0;
	layout[4].SemanticIndex = 0;
	layout[4].SemanticName = "BINORMAL";

	ThrowIfFailed(
		m_d3d11Device->CreateInputLayout(layout, ARRAYSIZE(layout),
			shaderPointer, shaderSize, &m_inputLayout)
	);
}

VertexShaderEnum SimpleVertexShader::getVertexShaderType() const
{
	return VertexShaderEnum::VertexShaderSimple;
}
