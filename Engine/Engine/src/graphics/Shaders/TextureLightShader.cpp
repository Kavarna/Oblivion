#include "TextureLightShader.h"
#include "../Direct3D11.h"

TextureLightShader::TextureLightShader()
{ };

TextureLightShader::~TextureLightShader()
{ };


void TextureLightShader::Create()
{
	try
	{
		ID3D11VertexShader ** VS = &m_vertexShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/TextureLightVertexShader.cso", "vs_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[0], reinterpret_cast<ID3D11DeviceChild**>(VS));

		D3D11_INPUT_ELEMENT_DESC elementDesc[7];
		elementDesc[0].AlignedByteOffset = 0;
		elementDesc[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		elementDesc[0].InputSlot = 0;
		elementDesc[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[0].InstanceDataStepRate = 0;
		elementDesc[0].SemanticIndex = 0;
		elementDesc[0].SemanticName = "POSITION";
		elementDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		elementDesc[1].InputSlot = 0;
		elementDesc[1].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[1].InstanceDataStepRate = 0;
		elementDesc[1].SemanticIndex = 0;
		elementDesc[1].SemanticName = "TEXCOORD";
		elementDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[2].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		elementDesc[2].InputSlot = 0;
		elementDesc[2].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[2].InstanceDataStepRate = 0;
		elementDesc[2].SemanticIndex = 0;
		elementDesc[2].SemanticName = "NORMAL";

		elementDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[3].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[3].InputSlot = 1;
		elementDesc[3].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[3].InstanceDataStepRate = 1;
		elementDesc[3].SemanticIndex = 0;
		elementDesc[3].SemanticName = "WORLDMATRIX";
		elementDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[4].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[4].InputSlot = 1;
		elementDesc[4].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[4].InstanceDataStepRate = 1;
		elementDesc[4].SemanticIndex = 1;
		elementDesc[4].SemanticName = "WORLDMATRIX";
		elementDesc[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[5].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[5].InputSlot = 1;
		elementDesc[5].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[5].InstanceDataStepRate = 1;
		elementDesc[5].SemanticIndex = 2;
		elementDesc[5].SemanticName = "WORLDMATRIX";
		elementDesc[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[6].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[6].InputSlot = 1;
		elementDesc[6].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[6].InstanceDataStepRate = 1;
		elementDesc[6].SemanticIndex = 3;
		elementDesc[6].SemanticName = "WORLDMATRIX";

		UINT arraySize = _countof(elementDesc);
		ThrowIfFailed(
			m_d3d11Device->CreateInputLayout(elementDesc, arraySize,
				m_shaderBlobs[0]->GetBufferPointer(), m_shaderBlobs[0]->GetBufferSize(),
				&m_inputLayout)
		);

		ID3D11PixelShader ** PS = &m_pixelShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/TextureLightPixelShader.cso", "ps_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[1], reinterpret_cast<ID3D11DeviceChild**>(PS));
		
		ShaderHelper::CreateBuffer(
			m_d3d11Device.Get(), &m_cameraBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(SCameraInfo), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
		);

		ShaderHelper::CreateBuffer(
			m_d3d11Device.Get(), &m_lightBuffer,
			D3D11_USAGE::D3D11_USAGE_DEFAULT,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(Sun), 0
		);

	}
	CATCH;
}

void TextureLightShader::SetLightInformations(Sun const & sun)
{
	m_d3d11Context->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &sun, 0, 0);
	m_d3d11Context->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());
}

void TextureLightShader::SetCameraInformations(SCameraInfo const & camInfo)
{
	ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_cameraBuffer.Get(),
		(void*)&camInfo, sizeof(SCameraInfo));
	m_d3d11Context->VSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
}

const Pipeline TextureLightShader::GetPipelineType() const
{
	return Pipeline::PipelineTextureLight;
}
