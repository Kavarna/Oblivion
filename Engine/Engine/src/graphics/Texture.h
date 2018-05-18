#pragma once

#include "WICTextureLoader.h"
#include "TextureUtilities.h"
#include "../common/common.h"




class CTexture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;

	bool mHasUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureUAV;

	ID3D11Device * mDevice;
public:
	CTexture() = default;
	CTexture(LPWSTR lpPath, ID3D11Device *, ID3D11DeviceContext *, bool hasUAV = false);
	~CTexture();
public:
	ID3D11ShaderResourceView* GetTextureSRV() const
	{
		return mTextureSRV.Get();
	}
	ID3D11UnorderedAccessView* GetTextureUAV() const
	{
		if (!mHasUAV)
			throw std::exception("Can't get a UAV from a texture that doesn't have one");
		return mTextureUAV.Get();

	}
	void SetTexture(ID3D11ShaderResourceView * newSRV)
	{
		mTextureSRV.Reset();
		mTextureSRV = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>(newSRV);
	}
private:
	void CreateUAV();
};