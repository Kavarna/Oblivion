#include "Font.h"
#include <atlbase.h>


CFont::CFont(LPWSTR lpPath)
{
	try
	{
		ReadFile(lpPath);
	}
	CATCH;
}

CFont::CFont(const char* lpPath)
{
	try
	{
		USES_CONVERSION;
		LPWSTR w = A2W(lpPath);
		ReadFile(w);
	}
	CATCH;
}


CFont::~CFont()
{
	mFPSTexture.reset();
}

void CFont::Build(void * Vertices, void * Indices, UINT & VertexCount, UINT & IndexCount, const std::wstring& Message, float x, float y)
{
	VertexCount = 0;
	IndexCount = 0;
	size_t len = Message.length();

	SVertex * vertices = (SVertex*)Vertices;
	DWORD * indices = (DWORD*)Indices;

	float StartX = x;

	int lastChar = -1;
	for (size_t i = 0; i < len; ++i)
	{
		int ch = Message[i];
		SLetter* letter = mLetters[ch];
		if (letter == 0)
		{
			letter = mLetters[(int)'='];
		}
		if (Message[i] == '\n')
		{
			y += mLineHeight;
			x = StartX;
		}
		int Kerning = GetKerning(lastChar, ch);

		int xOffset = (int)letter->xOffset + Kerning;

		// Top-left
		vertices[VertexCount].Position = DirectX::XMFLOAT3(x + xOffset, y - letter->yOffset, 1.0f);
		vertices[VertexCount].Texture = DirectX::XMFLOAT2(letter->u, letter->v);
		indices[IndexCount++] = VertexCount++;


		// Top-right
		vertices[VertexCount].Position = DirectX::XMFLOAT3(x + xOffset + letter->swidth, y - letter->yOffset, 1.0f);
		vertices[VertexCount].Texture = DirectX::XMFLOAT2(letter->u + letter->width, letter->v);
		indices[IndexCount++] = VertexCount++;


		// Bottom-right
		vertices[VertexCount].Position = DirectX::XMFLOAT3(x + xOffset + letter->swidth, y - letter->sheight - letter->yOffset, 1.0f);
		vertices[VertexCount].Texture = DirectX::XMFLOAT2(letter->u + letter->width, letter->v + letter->height);
		indices[IndexCount++] = VertexCount++;


		indices[IndexCount] = indices[IndexCount - 3];
		indices[IndexCount + 1] = indices[IndexCount - 1];
		IndexCount += 2;

		// Bottom-left
		vertices[VertexCount].Position = DirectX::XMFLOAT3(x + xOffset, y - letter->sheight - letter->yOffset, 1.0f);
		vertices[VertexCount].Texture = DirectX::XMFLOAT2(letter->u, letter->v + letter->height);
		indices[IndexCount++] = VertexCount++;

		x += letter->xAdvance;
	}
}

void CFont::ReadFile(LPWSTR Path)
{
	std::wifstream ifFile(Path);
	if (!ifFile.is_open())
		throw std::exception("Couldn't open font file");

	std::wstring temp;
	size_t startpos;
	ifFile >> temp >> temp; // info face=""
	startpos = temp.find(L"\"") + 1;
	if (temp[temp.size() - 1] != L'\"')
	{
		wchar_t ch = 0;
		while (ch != L'\"')
		{
			ifFile.get(ch);
		}
	}
	mName = temp.substr(startpos, temp.size() - startpos - 1);

	ifFile >> temp; // size=
	startpos = temp.find(L"=") + 1;
	mSize = std::stoi(temp.substr(startpos, temp.size() - startpos));

	ifFile >> temp >> temp >> temp >> temp >> temp >> temp >> temp; // bold,italic,charset,unicode,stretchH,smooth,aa

	ifFile >> temp; // padding
	startpos = temp.find(L"=") + 1;
	temp = temp.substr(startpos, temp.size() - startpos);

	startpos = temp.find(L",") + 1;
	mPadding[0] = std::stoi(temp.substr(0, startpos - 1));

	temp = temp.substr(startpos, temp.size() - startpos);
	startpos = temp.find(L",") + 1;
	mPadding[1] = std::stoi(temp.substr(0, startpos - 1));

	temp = temp.substr(startpos, temp.size() - startpos);
	startpos = temp.find(L",") + 1;
	mPadding[2] = std::stoi(temp.substr(0, startpos - 1));

	temp = temp.substr(startpos, temp.size() - startpos);
	startpos = temp.find(L",") + 1;
	mPadding[3] = std::stoi(temp.substr(0, startpos - 1));

	ifFile >> temp; // spacing

	ifFile >> temp; // common

	ifFile >> temp; // lineHeight
	startpos = temp.find(L"=") + 1;
	mLineHeight = std::stoi(temp.substr(startpos, temp.size() - startpos));

	ifFile >> temp; // base

	ifFile >> temp; // scaleW
	startpos = temp.find(L"=") + 1;
	mFPSTextureWidth = std::stoi(temp.substr(startpos, temp.size() - startpos));

	ifFile >> temp; // scaleH
	startpos = temp.find(L"=") + 1;
	mFPSTextureHeight = std::stoi(temp.substr(startpos, temp.size() - startpos));

	ifFile >> temp; // pages
	startpos = temp.find(L"=") + 1;
	if (std::stoi(temp.substr(startpos, temp.size() - startpos)) > 1)
		throw std::exception("Not ready for multiple pages font");

	ifFile >> temp; // packed

	ifFile >> temp; // page
	ifFile >> temp; // id

	ifFile >> temp; // file
	startpos = temp.find(L"\"") + 1;
	temp = temp.substr(startpos, temp.size() - startpos - 1);
	mFPSTexture = std::make_unique<Texture>((LPWSTR)temp.c_str(), false);

	int characterCount;
	ifFile >> temp >> temp;
	startpos = temp.find(L"=") + 1;
	characterCount = std::stoi(temp.substr(startpos, temp.size() - startpos));

	for (int i = 0; i < characterCount; ++i)
	{
		SLetter letter;

		ifFile >> temp; // char
		ifFile >> temp; // id=
		startpos = temp.find(L"=") + 1;
		letter.ID = std::stoi(temp.substr(startpos, temp.size() - startpos));

		ifFile >> temp; // x=
		startpos = temp.find(L"=") + 1;
		int x = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.u = float(x) / float(mFPSTextureWidth);
		ifFile >> temp; // y= 
		startpos = temp.find(L"=") + 1;
		int y = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.v = float(y) / float(mFPSTextureHeight);

		ifFile >> temp; // width=
		startpos = temp.find(L"=") + 1;
		int width = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.width = float(width) / float(mFPSTextureWidth);
		letter.swidth = float(width);

		ifFile >> temp; // height=
		startpos = temp.find(L"=") + 1;
		int height = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.height = float(height) / float(mFPSTextureHeight);
		letter.sheight = float(height);

		ifFile >> temp; // xoffset=
		startpos = temp.find(L"=") + 1;
		int xOffset = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.xOffset = (float)xOffset;// / float( mFPSTextureWidth );

		ifFile >> temp; // yoffset=
		startpos = temp.find(L"=") + 1;
		int yOffset = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.yOffset = (float)yOffset;// / float( mFPSTextureHeight );

		ifFile >> temp; // xadvance=

		startpos = temp.find(L"=") + 1;
		int xAdvange = std::stoi(temp.substr(startpos, temp.size() - startpos));
		letter.xAdvance = (float)xAdvange;

		ifFile >> temp; // page;
		ifFile >> temp; // chnl;

		mLetters[letter.ID] = new SLetter(letter);

	}

	ifFile >> temp; // kernings;
	ifFile >> temp; // count=

	int kerningCount;
	startpos = temp.find(L"=") + 1;
	kerningCount = std::stoi(temp.substr(startpos, temp.size() - startpos));
	mKernings.resize(kerningCount);
	for (int i = 0; i < kerningCount; ++i)
	{
		SKerning kerning;
		ifFile >> temp; // kerning

		ifFile >> temp; // first=
		startpos = temp.find(L"=") + 1;
		int first = std::stoi(temp.substr(startpos, temp.size() - startpos));
		kerning.first = first;

		ifFile >> temp; // second=
		startpos = temp.find(L"=") + 1;
		int second = std::stoi(temp.substr(startpos, temp.size() - startpos));
		kerning.second = second;

		ifFile >> temp; // amount=
		startpos = temp.find(L"=") + 1;
		int amount = std::stoi(temp.substr(startpos, temp.size() - startpos));
		kerning.amount = amount;

		mKernings[i] = kerning;
	}

	ifFile.close();
}

int CFont::GetKerning(int first, int second)
{
	for (auto & iter : mKernings)
	{
		if (iter.first == first && iter.second == second)
			return iter.amount;
	}
	return 0;
}
