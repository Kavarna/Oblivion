#include "Texture.h"
#include <atlbase.h>


Texture::Texture(LPWSTR lpPath, ID3D11Device * device, ID3D11DeviceContext * context, bool hasUAV) :
	mDevice(device)
{
	Create(lpPath, device, context, hasUAV);
}

Texture::Texture(LPSTR lpPath, ID3D11Device * device, ID3D11DeviceContext * context, bool hasUAV) :
	mDevice(device)
{
	USES_CONVERSION;
	LPWSTR lpwPath = A2W(lpPath);
	Create(lpwPath, device, context, hasUAV);
}

Texture::~Texture()
{
	mTextureSRV.Reset();
}

void Texture::Create(LPWSTR lpPath, ID3D11Device * device, ID3D11DeviceContext * context, bool hasUAV)
{
	if (hasUAV)
	{
		ThrowIfFailed(
			CreateWICTextureFromFile(device, context, lpPath,
				reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()), &mTextureSRV,
				D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		);
		CreateUAV();
		mHasUAV = true;
	}
	else
	{
		ThrowIfFailed(
			CreateWICTextureFromFile(device, context, lpPath,
				reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()), &mTextureSRV)
		);
	}
}

void Texture::CreateUAV()
{
	ID3D11Texture2D * texture;
	mTextureSRV->GetResource(reinterpret_cast<ID3D11Resource**>( &texture ));
	if (texture == nullptr)
		throw std::exception("Can't create a UAV from a null texture");

	TextureUtilities::CreateUAVFromTexture(mDevice, texture, &mTextureUAV);
}