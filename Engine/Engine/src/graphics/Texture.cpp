#include "Texture.h"
#include "Helpers/TextureUtilities.h"
#include <atlbase.h>

Texture::Texture(std::string path)
{
	USES_CONVERSION;
	LPWSTR lpwPath = A2W(path.c_str());
	Create(lpwPath, false);
}

Texture::Texture(float width, float height, bool hasUAV)
{
	mDimensions = { width,height };
	mHasUAV = hasUAV;
	UINT flags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	if (hasUAV)
		flags |= D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	TextureUtilities::CreateTexture(m_d3d11Device.Get(),
		&mTexture, (UINT)width, (UINT)height, flags,
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
		1, 0);

	TextureUtilities::CreateSRVFromTexture(m_d3d11Device.Get(), mTexture.Get(), &mTextureSRV);
	TextureUtilities::CreateUAVFromTexture(m_d3d11Device.Get(), mTexture.Get(), &mTextureUAV);
}

Texture::Texture(LPWSTR lpPath, bool hasUAV)
{
	Create(lpPath, hasUAV);
	USES_CONVERSION;
	LPSTR pathA = W2A(lpPath);
	mPath = std::string(pathA);
}

Texture::Texture(LPSTR lpPath, bool hasUAV)
{
	USES_CONVERSION;
	LPWSTR lpwPath = A2W(lpPath);
	Create(lpwPath, hasUAV);
	mPath = std::string(lpPath);
}

Texture::Texture(ID3D11Texture2D * tex, ID3D11ShaderResourceView * srv, ID3D11UnorderedAccessView * uav)
{
	mHasUAV = false;
	mTexture.Reset();
	mTexture = MicrosoftPointer<ID3D11Texture2D>(tex);

	mTextureSRV.Reset();
	mTextureSRV = MicrosoftPointer<ID3D11ShaderResourceView>(srv);

	if (uav)
	{
		mHasUAV = true;
		mTextureUAV.Reset();
		mTextureUAV = MicrosoftPointer<ID3D11UnorderedAccessView>(uav);
	}

	D3D11_TEXTURE2D_DESC texDesc;
	mTexture->GetDesc(&texDesc);
	mDimensions = { (float)texDesc.Width,(float)texDesc.Height };
}

Texture::~Texture()
{
	mTextureSRV.Reset();
}

void Texture::Create(LPWSTR lpPath, bool hasUAV)
{
	auto extension = StrRChrW(lpPath, lpPath + lstrlenW(lpPath), L'.');
	if (StrCmpW(extension, L".dds") == 0)
	{
		if (hasUAV)
		{
			ThrowIfFailed(
				DirectX::CreateDDSTextureFromFile(m_d3d11Device.Get(), m_d3d11Context.Get(), lpPath,
					reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()),
					&mTextureSRV, D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
			);
			CreateUAV();
			mHasUAV = true;
		}
		else
		{
			ThrowIfFailed(
				DirectX::CreateDDSTextureFromFile(m_d3d11Device.Get(), m_d3d11Context.Get(), lpPath,
					reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()),
					&mTextureSRV)
			);
		}
	}
	else
	{
		if (hasUAV)
		{
			ThrowIfFailed(
				CreateWICTextureFromFile(m_d3d11Device.Get(), m_d3d11Context.Get(), lpPath,
					reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()), &mTextureSRV,
					D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
			);
			CreateUAV();
			mHasUAV = true;
		}
		else
		{
			ThrowIfFailed(
				CreateWICTextureFromFile(m_d3d11Device.Get(), m_d3d11Context.Get(), lpPath,
					reinterpret_cast<ID3D11Resource**>(mTexture.GetAddressOf()), &mTextureSRV)
			);
		}
	}
	D3D11_TEXTURE2D_DESC texDesc;
	mTexture->GetDesc(&texDesc);
	mDimensions = { (float)texDesc.Width,(float)texDesc.Height };
}

const CommonTypes::fDimension& Texture::GetDimensions() const 
{
	return mDimensions;
}

void Texture::CreateUAV()
{
	ID3D11Texture2D * texture;
	mTextureSRV->GetResource(reinterpret_cast<ID3D11Resource**>( &texture ));
	if (texture == nullptr)
		throw std::exception("Can't create a UAV from a null texture");

	TextureUtilities::CreateUAVFromTexture(m_d3d11Device.Get(), texture, &mTextureUAV);
}