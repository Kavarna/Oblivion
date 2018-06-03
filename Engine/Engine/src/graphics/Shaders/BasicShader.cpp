#include "BasicShader.h"
#include "../Direct3D11.h"

std::once_flag					BasicShader::m_shaderFlags;
std::unique_ptr<BasicShader>	BasicShader::m_shaderInstance;

BasicShader::BasicShader()
{
	auto renderer = Direct3D11::Get();
	m_d3d11Device = renderer->getDevice();
	m_d3d11Context = renderer->getContext();
}

BasicShader::~BasicShader()
{ };

void BasicShader::Create()
{
	try
	{
		ID3D11VertexShader ** VS = &m_vertexShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/BasicVertexShader.cso", "vs_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[0], reinterpret_cast<ID3D11DeviceChild**>(VS));

		D3D11_INPUT_ELEMENT_DESC elementDesc[1];
		elementDesc[0].AlignedByteOffset = 0;
		elementDesc[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		elementDesc[0].InputSlot = 0;
		elementDesc[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[0].InstanceDataStepRate = 0;
		elementDesc[0].SemanticIndex = 0;
		elementDesc[0].SemanticName = "POSITION";
		UINT arraySize = _countof(elementDesc);
		ThrowIfFailed(
			m_d3d11Device->CreateInputLayout(elementDesc, arraySize,
				m_shaderBlobs[0]->GetBufferPointer(), m_shaderBlobs[0]->GetBufferSize(),
				&m_layout)
			);

		ID3D11PixelShader ** PS = &m_pixelShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/BasicPixelShader.cso", "ps_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[1], reinterpret_cast<ID3D11DeviceChild**>(PS));

		ShaderHelper::CreateBuffer(
			m_d3d11Device.Get(), &m_cameraBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(SCameraInfo),
			D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
		);
	}
	CATCH;
}

void BasicShader::bind() const
{
#if DEBUG || _DEBUG
	if (IShader::shouldBind<BasicShader>())
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

void BasicShader::SetCameraInformations(SCameraInfo const & info) const
{
	ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_cameraBuffer.Get(),
		(void*)&info, sizeof(SCameraInfo));
	m_d3d11Context->VSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
}

BasicShader* BasicShader::Get()
{
	std::call_once(m_shaderFlags, [&] { m_shaderInstance = std::make_unique<BasicShader>(); m_shaderInstance->Create(); });
	return m_shaderInstance.get();
}