#include "DisplacementShader.h"
#include "../Direct3D11.h"

DECLARE_SINGLETONE(DisplacementShader);


DisplacementShader::DisplacementShader()
{
}


DisplacementShader::~DisplacementShader()
{
}

void DisplacementShader::Create()
{
	try
	{
		ID3D11VertexShader ** VS = &m_vertexShader;
		ShaderHelper::CreateShaderFromFile((LPWSTR)L"Shaders/DisplacementMappingVertexShader.cso", (LPSTR)"vs_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[0], reinterpret_cast<ID3D11DeviceChild**>(VS));

		D3D11_INPUT_ELEMENT_DESC elementDesc[9];
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
		elementDesc[3].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		elementDesc[3].InputSlot = 0;
		elementDesc[3].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[3].InstanceDataStepRate = 0;
		elementDesc[3].SemanticIndex = 0;
		elementDesc[3].SemanticName = "TANGENT";
		elementDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[4].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		elementDesc[4].InputSlot = 0;
		elementDesc[4].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc[4].InstanceDataStepRate = 0;
		elementDesc[4].SemanticIndex = 0;
		elementDesc[4].SemanticName = "BINORMAL";

		elementDesc[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[5].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[5].InputSlot = 1;
		elementDesc[5].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[5].InstanceDataStepRate = 1;
		elementDesc[5].SemanticIndex = 0;
		elementDesc[5].SemanticName = "WORLDMATRIX";
		elementDesc[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[6].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[6].InputSlot = 1;
		elementDesc[6].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[6].InstanceDataStepRate = 1;
		elementDesc[6].SemanticIndex = 1;
		elementDesc[6].SemanticName = "WORLDMATRIX";
		elementDesc[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[7].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[7].InputSlot = 1;
		elementDesc[7].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[7].InstanceDataStepRate = 1;
		elementDesc[7].SemanticIndex = 2;
		elementDesc[7].SemanticName = "WORLDMATRIX";
		elementDesc[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc[8].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		elementDesc[8].InputSlot = 1;
		elementDesc[8].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
		elementDesc[8].InstanceDataStepRate = 1;
		elementDesc[8].SemanticIndex = 3;
		elementDesc[8].SemanticName = "WORLDMATRIX";

		UINT arraySize = _countof(elementDesc);
		ThrowIfFailed(
			m_d3d11Device->CreateInputLayout(elementDesc, arraySize,
				m_shaderBlobs[0]->GetBufferPointer(), m_shaderBlobs[0]->GetBufferSize(),
				&m_inputLayout)
		);

		ID3D11HullShader ** HS = &m_hullShader;
		ShaderHelper::CreateShaderFromFile((LPWSTR)L"Shaders/DisplacementMappingHullShader.cso", (LPSTR)"hs_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[1], reinterpret_cast<ID3D11DeviceChild**>(HS));

		ID3D11DomainShader ** DS = &m_domainShader;
		ShaderHelper::CreateShaderFromFile((LPWSTR)L"Shaders/DisplacementMappingDomainShader.cso", (LPSTR)"ds_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[2], reinterpret_cast<ID3D11DeviceChild**>(DS));

		ID3D11PixelShader ** PS = &m_pixelShader;
		ShaderHelper::CreateShaderFromFile((LPWSTR)L"Shaders/DisplacementMappingPixelShader.cso", (LPSTR)"ps_4_0",
			m_d3d11Device.Get(), &m_shaderBlobs[3], reinterpret_cast<ID3D11DeviceChild**>(PS));

		ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_cameraBufferVS,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(VSCameraBuffer), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);

		ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_cameraBufferDS,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof(DSCameraBuffer), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
	}
	CATCH;
}

const Pipeline DisplacementShader::GetPipelineType() const
{
	return Pipeline::PipelineDisplacementTextureLight;
}

void DisplacementShader::SetCameraInfo(ICamera * cam)
{
	auto dataVS = (VSCameraBuffer*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_cameraBufferVS.Get());

	dataVS->eyePosition			= cam->GetPosition();
	dataVS->minTessDistance		= cam->GetFarZ();
	dataVS->maxTessDistance		= cam->GetNearZ();

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_cameraBufferVS.Get());

	auto dataDS = (DSCameraBuffer*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_cameraBufferDS.Get());

	dataDS->View		= DirectX::XMMatrixTranspose(cam->GetView());
	dataDS->Projection	= DirectX::XMMatrixTranspose(cam->GetProjection());

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_cameraBufferDS.Get());

	ID3D11Buffer * buffers[2] =
	{
		m_cameraBufferDS.Get(),
		m_cameraBufferVS.Get(),
	};

	ShaderHelper::BindConstantBuffer(m_d3d11Context.Get(), 0, 2, buffers,
		(uint32_t)Shader::ShaderType::eVertex);

	buffers[0] = m_cameraBufferDS.Get();
	ShaderHelper::BindConstantBuffer(m_d3d11Context.Get(), 0, 1, buffers,
		(uint32_t)Shader::ShaderType::eDomain);
}
