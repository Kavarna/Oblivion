#pragma once

#include "WICTextureLoader.h"
#include "Helpers/TextureUtilities.h"
#include "../common/common.h"




class Texture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;

	bool mHasUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureUAV;

	ID3D11Device * mDevice;
public:
	Texture() = default;
	Texture(LPWSTR lpPath, ID3D11Device *, ID3D11DeviceContext *, bool hasUAV = false);
	Texture(LPSTR lpPath, ID3D11Device *, ID3D11DeviceContext *, bool hasUAV = false);
	~Texture();
private:
	void Create(LPWSTR lpPath, ID3D11Device *, ID3D11DeviceContext *, bool hasUAV);
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
	void SetTexture(ID3D11ShaderResourceView * newSRV, ID3D11UnorderedAccessView * newUAV = nullptr)
	{
		mHasUAV = false;
		mTextureSRV.Reset();
		mTextureSRV = Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>(newSRV);
		if (newUAV)
		{
			mHasUAV = true;
			mTextureUAV.Reset();
			mTextureUAV = Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>(newUAV);
		}
	}
private:
	void CreateUAV();
};