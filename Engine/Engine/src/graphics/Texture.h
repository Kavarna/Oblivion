#pragma once

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "Helpers/TextureUtilities.h"
#include "../common/common.h"
#include "interfaces/GraphicsObject.h"




class Texture : public IGraphicsObject
{
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;

	bool mHasUAV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureUAV;

	std::string mPath;
public:
	Texture() = default;
	Texture(std::string);
	Texture(float width, float height, bool hasUAV);
	Texture(LPWSTR lpPath, bool hasUAV = false);
	Texture(LPSTR lpPath, bool hasUAV = false);
	Texture(ID3D11Texture2D *, ID3D11ShaderResourceView *, ID3D11UnorderedAccessView * = nullptr);
	~Texture();
private:
	void Create(LPWSTR lpPath, bool hasUAV);
public:
	std::string GetPath() const
	{
		return mPath;
	}
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
