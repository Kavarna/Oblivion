#include "SimpleTextureShader.h"

DECLARE_SINGLETONE(SimpleTextureShader);


SimpleTextureShader::SimpleTextureShader()
{
}

SimpleTextureShader::~SimpleTextureShader()
{
}

void SimpleTextureShader::Create()
{
	try
	{
		ID3D11VertexShader ** VS = &m_vertexShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/SimpleTextureVertexShader.cso", "vs_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[0], reinterpret_cast<ID3D11DeviceChild**>(VS));

		D3D11_INPUT_ELEMENT_DESC elementDesc[2];
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

		UINT arraySize = sizeof(elementDesc) / sizeof(elementDesc[0]);
		ThrowIfFailed(
			m_d3d11Device->CreateInputLayout(elementDesc, arraySize,
				m_shaderBlobs[0]->GetBufferPointer(), m_shaderBlobs[0]->GetBufferSize(),
				&m_inputLayout)
		);

		ID3D11PixelShader ** PS = &m_pixelShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/SimpleTexturePixelShader.cso", "ps_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[1], reinterpret_cast<ID3D11DeviceChild**>(PS));


		ShaderHelper::CreateBuffer(
			m_d3d11Device.Get(), &m_cameraBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(DirectX::XMMATRIX), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
		);
		DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
		ShaderHelper::CreateBuffer(
			m_d3d11Device.Get(), &m_colorBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(DirectX::XMFLOAT4), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE,
			&color
		);

	}
	CATCH;
}

const Pipeline SimpleTextureShader::GetPipelineType() const
{
	return Pipeline::PipelineSimpleTexture;
}

void SimpleTextureShader::SetCameraInfo(DirectX::FXMMATRIX & WVP)
{
	auto data = ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_cameraBuffer.Get());
	memcpy(data, &WVP, sizeof(DirectX::XMMATRIX));
	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_cameraBuffer.Get());
	m_d3d11Context->VSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
}

void SimpleTextureShader::SetColor(const DirectX::XMFLOAT4 color)
{
	auto data = ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_colorBuffer.Get());
	memcpy(data, &color, sizeof(DirectX::XMFLOAT4));
	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_colorBuffer.Get());
	m_d3d11Context->PSSetConstantBuffers(13, 1, m_colorBuffer.GetAddressOf());
}
