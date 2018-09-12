#pragma once

#include "Font.h"
#include "interfaces/I2DTransforms.h"
#include "interfaces/ICamera.h"

class Text sealed : public IGraphicsObject, public I2DTransforms
{
	MicrosoftPointer<ID3D11Buffer> mVertexBuffer;
	MicrosoftPointer<ID3D11Buffer> mIndexBuffer;
	MicrosoftPointer<ID3D11Buffer> mMaterialBuffer;

	std::shared_ptr<CFont> mFont;

	float mLastX = -1;
	float mLastY = -1;
	LPWSTR mLastText = L"";

	UINT mSize;

	UINT mIndexCount = 0;
	UINT mVertexCount = 0;

public:
	Text(std::shared_ptr<CFont> Font,
		float WindowWidth, float WindowHeight,
		UINT MaxLength = 128);
	~Text();
public:
	void Render(ICamera* cam, const std::wstring& Text, float X, float Y,
		DirectX::XMFLOAT4 Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	void ForceUpdate();
	const CFont* GetFont() const { return mFont.get(); }
private:
	void InitializeBuffers(UINT length);
	void Render(ICamera *, const DirectX::XMFLOAT4& color);
};