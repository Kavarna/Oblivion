#pragma once

#include "../common/common.h"
#include "ShaderHelper.h"

namespace TextureUtilities
{
	/// <summary>Creates an unordered access view for a texture. Note: The texture must have D3D11_BIND_UNORDERED_ACCESS bind flag</summary>
	inline void CreateUAVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11UnorderedAccessView ** finalResult, UINT mipSlice = 0)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
		uavDesc.Format = texDesc.Format;
		uavDesc.Texture2D.MipSlice = mipSlice;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateUnorderedAccessView(texture, &uavDesc, finalResult)
			);
	}

	/// <summary>Creates a shader resource view for a texture. Note: The texture must have D3D11_BIND_SHADER_RESOURCE bind flag</summary>
	inline void CreateSRVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11ShaderResourceView ** finalResult)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
		srvDesc.Format = texDesc.Format;
		if (texDesc.SampleDesc.Quality == 0)
		{
			srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		else
		{
			srvDesc.Texture2DMS;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}


		ThrowIfFailed(
			device->CreateShaderResourceView(texture, &srvDesc, finalResult)
			);
	}

	/// <summary>Creates a render target view for a texture. Note: The texture must have D3D11_RENDER_TARGET bind flag</summary>
	inline void CreateRTVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11RenderTargetView ** finalResult, UINT mipSlice = 0) 
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_RENDER_TARGET_VIEW_DESC, rtvDesc);
		rtvDesc.Format = texDesc.Format;
		if (texDesc.SampleDesc.Quality == 0)
		{
			rtvDesc.Texture2D.MipSlice = mipSlice;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		}
		else
		{
			rtvDesc.Texture2DMS;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}

		ThrowIfFailed(
			device->CreateRenderTargetView(texture, &rtvDesc, finalResult)
		);
	}

	/// <summary>Creates a depth stencil view for a texture. Note: The texture must have D3D11_DEPTH_STENCIL bind flag and an according format</summary>
	inline void CreateDSVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11DepthStencilView ** finalResult, UINT mipSlice = 0)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_DEPTH_STENCIL_VIEW_DESC, depthDesc);
		depthDesc.Format = texDesc.Format;
		if (texDesc.SampleDesc.Quality == 0)
		{
			depthDesc.Texture2D.MipSlice = 0;
			depthDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		}
		else
		{
			depthDesc.Texture2DMS;
			depthDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}

		ThrowIfFailed(
			device->CreateDepthStencilView(texture, &depthDesc, finalResult)
		);
	}

	/// <summary>Creates a new empty texture</summary>
	inline void CreateTexture(ID3D11Device * device, ID3D11Texture2D  ** resultTexture,
		UINT width, UINT height,
		UINT bindFlags, DXGI_FORMAT format,
		UINT MSAACount, UINT MSAAQuality,
		D3D11_USAGE usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
		UINT CPUAccessFlag = 0, UINT mipLevels = 1, UINT arraySize = 1,
		UINT miscFlag = 0)
	{
		ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
		texDesc.ArraySize = arraySize;
		texDesc.BindFlags = bindFlags;
		texDesc.CPUAccessFlags = CPUAccessFlag;
		texDesc.Format = format;
		texDesc.Height = height;
		texDesc.Width = width;
		texDesc.MipLevels = mipLevels;
		texDesc.MiscFlags = miscFlag;
		texDesc.Usage = usage;
		texDesc.SampleDesc.Count = MSAACount;
		texDesc.SampleDesc.Quality = MSAAQuality;
		ThrowIfFailed(
			device->CreateTexture2D(&texDesc, nullptr, resultTexture)
		);
	}

}