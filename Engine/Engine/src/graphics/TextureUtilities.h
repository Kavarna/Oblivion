#pragma once

#include "../common/common.h"
#include "ShaderHelper.h"

namespace TextureUtilities
{

	inline void CreateUAVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11UnorderedAccessView ** finalResult)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
		uavDesc.Format = texDesc.Format;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateUnorderedAccessView(texture, &uavDesc, finalResult)
			);
	}

	inline void CreateSRVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11ShaderResourceView ** finalResult)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
		srvDesc.Format = texDesc.Format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateShaderResourceView(texture, &srvDesc, finalResult)
			);
	}



}