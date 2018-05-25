#include "BasicShader.h"

BasicShader::BasicShader() :
	IShader(1)
{

}

BasicShader::~BasicShader()
{ };

void BasicShader::Create(ID3D11Device * device)
{
	try
	{
		ID3D11VertexShader ** VS = &m_vertexShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/BasicVertexShader.cso", "vs_4_0",
			device, &m_shaderBlobs[0], reinterpret_cast<ID3D11DeviceChild**>( VS ));

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
			device->CreateInputLayout(elementDesc, arraySize,
				m_shaderBlobs[0]->GetBufferPointer(), m_shaderBlobs[0]->GetBufferSize(),
				&m_layout)
			);

		ID3D11PixelShader ** PS = &m_pixelShader;
		ShaderHelper::CreateShaderFromFile(L"Shaders/BasicPixelShader.cso", "ps_4_0",
			device, &m_shaderBlobs[1], reinterpret_cast<ID3D11DeviceChild**>( PS ));

		ShaderHelper::CreateBuffer(device, &m_cameraBuffer, D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, sizeof(IShader::SCameraInformations),
			D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
	}
	CATCH;
}

void BasicShader::bind(ID3D11DeviceContext * context) const
{
	context->IASetInputLayout(m_layout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void BasicShader::bindCameraInformations(ID3D11DeviceContext* context, IShader::SCameraInformations const* camInfo) const
{
	static D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	typedef IShader::SCameraInformations CameraInformations;
	ThrowIfFailed(
		context->Map(m_cameraBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)
		);

	memcpy_s(mappedSubresource.pData, sizeof(CameraInformations), (void*)camInfo, sizeof(CameraInformations));

	context->Unmap(m_cameraBuffer.Get(), 0);
	context->VSSetConstantBuffers(0, 1, m_cameraBuffer.GetAddressOf());
}
