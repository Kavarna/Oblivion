#pragma once

#include "../common/common.h"


namespace ShaderHelper
{

	inline void CreateShaderFromFile(LPWSTR filePath, LPSTR profile,
		ID3D11Device * device, ID3DBlob ** ShaderBlob, ID3D11DeviceChild ** Shader)
	{

		ThrowIfFailed(D3DReadFileToBlob(filePath, ShaderBlob));

		switch (profile[0])
		{
		case 'v': // Vertex Shader
		ThrowIfFailed(
			device->CreateVertexShader(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(), nullptr,
				reinterpret_cast< ID3D11VertexShader** >( Shader ))
			);
		break;
		case 'h': // Hull shader
		ThrowIfFailed(
			device->CreateHullShader(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(), nullptr,
				reinterpret_cast< ID3D11HullShader** > ( Shader ))
			);
		break;
		case 'd': // Domain Shader
		ThrowIfFailed(
			device->CreateDomainShader(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(), nullptr,
				reinterpret_cast< ID3D11DomainShader** > ( Shader ))
			);
		break;
		case 'g': // Geometry Shader
		ThrowIfFailed(
			device->CreateGeometryShader(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(), nullptr,
				reinterpret_cast< ID3D11GeometryShader** > ( Shader ))
			);
		break;
		case 'p': // Pixel Shader
		ThrowIfFailed(
			device->CreatePixelShader(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(), nullptr,
				reinterpret_cast< ID3D11PixelShader** > ( Shader ))
			);
		break;
		case 'c':
		ThrowIfFailed(
			device->CreateComputeShader(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(), nullptr,
				reinterpret_cast< ID3D11ComputeShader** > ( Shader ))
			);
		break;
		}

	}

	inline void CreateStreamOutputEntries(D3D11_INPUT_ELEMENT_DESC * layout, unsigned int numLayout,
		D3D11_SO_DECLARATION_ENTRY *& soe)
	{
		soe = new D3D11_SO_DECLARATION_ENTRY[numLayout];
		for (size_t i = 0; i < numLayout; ++i)
		{
			soe[i].ComponentCount = DX::GetComponentCountFromFormat(layout[i].Format);
			soe[i].OutputSlot = 0;
			soe[i].SemanticIndex = layout[i].SemanticIndex;
			soe[i].SemanticName = layout[i].SemanticName;
			soe[i].StartComponent = 0;
			soe[i].Stream = 0;
		}
	}

	inline void CreateGeometryShaderWithStreamOutput(LPWSTR filePath, LPSTR profile,
		ID3D11Device * device, ID3DBlob ** ShaderBlob, ID3D11DeviceChild ** Shader,
		D3D11_SO_DECLARATION_ENTRY *& soe, unsigned int numLayout)
	{
		ThrowIfFailed(D3DReadFileToBlob(filePath, ShaderBlob));

		D3D_FEATURE_LEVEL featureLevel = device->GetFeatureLevel();
		if (featureLevel >= D3D_FEATURE_LEVEL_11_0)
		{
			ThrowIfFailed(device->CreateGeometryShaderWithStreamOutput(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(),
				soe, numLayout, nullptr, 0, D3D11_SO_NO_RASTERIZED_STREAM,
				nullptr, reinterpret_cast< ID3D11GeometryShader** >( Shader )
				));
		}
		else
		{
			ThrowIfFailed(device->CreateGeometryShaderWithStreamOutput(( *ShaderBlob )->GetBufferPointer(), ( *ShaderBlob )->GetBufferSize(),
				soe, numLayout, nullptr, 0, 0,
				nullptr, reinterpret_cast< ID3D11GeometryShader** >( Shader )
				));
		}
	}

	inline void CreateGeometryShaderWithStreamOutput(LPWSTR filePath, LPSTR profile,
		ID3D11Device * device, ID3DBlob ** ShaderBlob, ID3D11DeviceChild ** Shader,
		D3D11_INPUT_ELEMENT_DESC *layout, unsigned int numLayout, D3D11_SO_DECLARATION_ENTRY *& soe)
	{
		CreateStreamOutputEntries(layout, numLayout, soe);

		CreateGeometryShaderWithStreamOutput(filePath, profile,
			device, ShaderBlob, Shader, soe, numLayout);
	}

	inline void* MapBuffer(ID3D11DeviceContext * context, ID3D11Buffer * buffer,
		D3D11_MAP mapType = D3D11_MAP::D3D11_MAP_WRITE_DISCARD, uint16_t subResource = 0)
	{
		static D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ThrowIfFailed(
			context->Map(buffer, subResource, mapType, 0, &mappedSubresource)
		);
		return mappedSubresource.pData;
	}

	inline void UnmapBuffer(ID3D11DeviceContext * context, ID3D11Buffer * buffer,
		uint16_t subResource = 0)
	{
		context->Unmap(buffer, subResource);
	}

	inline void CreateBuffer(ID3D11Device * device, ID3D11Buffer ** FinalBuffer,
		D3D11_USAGE Usage, D3D11_BIND_FLAG flag, size_t DataSize, int CPUAccessFlag,
		void* data = nullptr, UINT MiscFlag = 0, UINT StructureByteStride = 0)
	{
		ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, buffDesc);
		ZeroMemoryAndDeclare(D3D11_SUBRESOURCE_DATA, buffData);
		buffDesc.BindFlags = flag;
		buffDesc.ByteWidth = (UINT)DataSize;
		buffDesc.CPUAccessFlags = CPUAccessFlag;
		buffDesc.MiscFlags = MiscFlag;
		buffDesc.StructureByteStride = StructureByteStride;
		buffDesc.Usage = Usage;
		if (data /*!= nullptr*/)
		{
			buffData.pSysMem = data;
			ThrowIfFailed(
				device->CreateBuffer(&buffDesc, &buffData, FinalBuffer)
				);
		}
		else
		{
			ThrowIfFailed(
				device->CreateBuffer(&buffDesc, nullptr, FinalBuffer)
				);
		}
	}
}