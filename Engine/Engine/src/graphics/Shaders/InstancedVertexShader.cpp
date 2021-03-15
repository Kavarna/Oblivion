#include "InstancedVertexShader.h"

InstancedVertexShader::InstancedVertexShader() :
	IVertexShader((LPWSTR)L"Shaders/InstancedVertexShader.cso")
{
	m_cameraBuffer = BufferManager::Get()->CreateBuffer(
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, sizeof(SCameraInfo),
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	);
	m_perCameraBuffer = BufferManager::Get()->CreateBuffer(
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, sizeof(SAdditionalCameraInfo),
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	);
	m_additionalCameraBuffer = BufferManager::Get()->CreateBuffer(
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, sizeof(SCameraInfo),
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	);
}

void __vectorcall InstancedVertexShader::SetCamera(const SCameraInfo & WVP)
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->MapBuffer(m_cameraBuffer, (void*)&WVP);
	bufferManager->bindVSBuffer(0, m_cameraBuffer);
}

void InstancedVertexShader::SetCameraAdditionalInfo(const SAdditionalCameraInfo& ad)
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->MapBuffer(m_perCameraBuffer, (void*)&ad);
	bufferManager->bindVSBuffer(1, m_perCameraBuffer);
}

void __vectorcall InstancedVertexShader::SetAdditionalCamera(const SCameraInfo & WVP)
{
	static auto bufferManager = BufferManager::Get();
	bufferManager->MapBuffer(m_additionalCameraBuffer, (void*)&WVP);
	bufferManager->bindVSBuffer(3, m_additionalCameraBuffer);
}

void InstancedVertexShader::Create()
{
	createInputLayout();
}

void InstancedVertexShader::createInputLayout()
{
	LPVOID shaderPointer = getShaderBlobPointer();
	SIZE_T shaderSize = getShaderBlobSize();

	D3D11_INPUT_ELEMENT_DESC layout[9];
	layout[0].AlignedByteOffset = 0;
	layout[0].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;
	layout[0].SemanticIndex = 0;
	layout[0].SemanticName = "POSITION";

	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
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

	layout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[5].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[5].InputSlot = 1;
	layout[5].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[5].InstanceDataStepRate = 1;
	layout[5].SemanticIndex = 0;
	layout[5].SemanticName = "WORLDMATRIX";
	layout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[6].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[6].InputSlot = 1;
	layout[6].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[6].InstanceDataStepRate = 1;
	layout[6].SemanticIndex = 1;
	layout[6].SemanticName = "WORLDMATRIX";
	layout[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[7].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[7].InputSlot = 1;
	layout[7].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[7].InstanceDataStepRate = 1;
	layout[7].SemanticIndex = 2;
	layout[7].SemanticName = "WORLDMATRIX";
	layout[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[8].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[8].InputSlot = 1;
	layout[8].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
	layout[8].InstanceDataStepRate = 1;
	layout[8].SemanticIndex = 3;
	layout[8].SemanticName = "WORLDMATRIX";

	ThrowIfFailed(
		m_d3d11Device->CreateInputLayout(layout, ARRAYSIZE(layout),
			shaderPointer, shaderSize, &m_inputLayout)
	);
}

VertexShaderEnum InstancedVertexShader::getVertexShaderType() const
{
	return VertexShaderEnum::VertexShaderInstanced;
}
