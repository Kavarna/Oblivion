#pragma once

#include "../common/common.h"
#include "interfaces/GraphicsObject.h"
#include "Texture.h"


class CFont sealed : public IGraphicsObject
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
	};
	struct SLetter
	{
		short ID;
		float u; // (0-1) - Start U
		float v; // (0-1) - Start V
		float width;
		float height;
		float xOffset;
		float yOffset;
		float xAdvance; // Advance X pixels when rendering
		float swidth;
		float sheight;
	};
	struct SKerning
	{
		int first; // First character
		int second; // Second character
		int amount; // Pixels
	};
private:
	std::shared_ptr<Texture> mFPSTexture;
	std::unordered_map<int, SLetter*> mLetters;
	std::vector<SKerning> mKernings;
	std::array<int, 4> mPadding;
	std::wstring mName;
	int mSize;
	int mLineHeight;
	int mFPSTextureWidth;
	int mFPSTextureHeight;

public:
	CFont(LPWSTR lpPath);
	CFont(const char* lpPath);
	~CFont();
public:
	std::shared_ptr<Texture> GetTexture() { return mFPSTexture; };
	void Build(void* Vertices, void* Indices,
		UINT& VertexCount, UINT& IndexCount,
		const std::wstring& Message, float x, float y);
public:
	inline int GetHeight() const { return mLineHeight; };
private:
	void ReadFile(LPWSTR);
	int GetKerning(int first, int second);
};

