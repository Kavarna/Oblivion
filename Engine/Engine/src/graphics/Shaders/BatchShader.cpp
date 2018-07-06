#include "BatchShader.h"
#include "BatchShader.h"
#include "../Direct3D11.h"

DECLARE_SINGLETONE(BatchShader);


BatchShader::BatchShader()
{
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_objectBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(DirectX::XMMATRIX), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}


BatchShader::~BatchShader()
{
}

void BatchShader::Create()
{
	try
	{
		ID3D11VertexShader ** VS = &m_vertexShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/BatchVertexShader.cso", "vs_4_0",
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
		elementDesc[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[1].InputSlot = 0;
		elementDesc[1].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[1].InstanceDataStepRate = 0;
		elementDesc[1].SemanticIndex = 0;
		elementDesc[1].SemanticName = "COLOR";
		UINT arraySize = _countof(elementDesc);
		ThrowIfFailed(
			m_d3d11Device->CreateInputLayout(elementDesc, arraySize,
				m_shaderBlobs[0]->GetBufferPointer(), m_shaderBlobs[0]->GetBufferSize(),
				&m_layout)
		);

		ID3D11PixelShader ** PS = &m_pixelShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/BatchPixelShader.cso", "ps_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[0], reinterpret_cast<ID3D11DeviceChild**>(PS));
	}
	CATCH;
}

void BatchShader::bind() const
{
#if DEBUG || _DEBUG
	if (IShader::shouldBind<BatchShader>())
#endif
	{
		m_d3d11Context->IASetInputLayout(m_layout.Get());
		m_d3d11Context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		m_d3d11Context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	}
#if DEBUG || _DEBUG
	else
	{
		DX::OutputVDebugString(L"[LOG]: Attempting to bind the same shader multiple times. This might be a performance hit.\n");
	}
#endif
}

void BatchShader::SetCamera(DirectX::FXMMATRIX & mat)
{
	struct buffer
	{
		DirectX::XMMATRIX VP;
	};

	auto data = (buffer*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_objectBuffer.Get());

	data->VP = mat;

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_objectBuffer.Get());

	m_d3d11Context->VSSetConstantBuffers(0, 1, m_objectBuffer.GetAddressOf());
}
