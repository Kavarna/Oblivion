#include "Text.h"
#include "interfaces/Shader.h"
#include "Pipelines/TexturePipeline.h"
#include "OblivionObjects.h"

Text::Text(std::shared_ptr<CFont> Font,
	float WindowWidth, float WindowHeight,
	UINT maxLength) :
	mFont(Font),
	mSize(maxLength),
	I2DTransforms(WindowWidth, WindowHeight)
{
	try
	{
		InitializeBuffers(maxLength);
	}
	CATCH;
}


Text::~Text()
{
	mFont.reset();
}


void Text::InitializeBuffers(UINT length)
{
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), mVertexBuffer.GetAddressOf(),
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof(CFont::SVertex) * 4 * length, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), mIndexBuffer.GetAddressOf(),
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(DWORD) * 6 * length, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
	
	ZeroMemoryAndDeclare(Shader::material_t, data);
	data.hasTexture = TRUE;
	/*ShaderHelper::CreateBuffer(m_d3d11Device.Get(), mMaterialBuffer.GetAddressOf(),
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(Shader::material_t), 0, &data);*/
	Shader::material_t material;
	material.hasTexture = TRUE;
	mMaterialBuffer = BufferManager::Get()->CreateBuffer(D3D11_USAGE::D3D11_USAGE_IMMUTABLE,
		sizeof(Shader::material_t), 0, &material);
}


void Text::Render(ICamera * cam, const std::wstring& Text, float X, float Y, DirectX::XMFLOAT4 Color)
{
	if (X == mLastX && Y == mLastY &&
		(lstrcmpW(mLastText, Text.c_str()) == 0))
	{
		Render(cam, Color);
		return;
	}

	if (Text.length() > mSize)
	{
		mSize = (UINT)Text.length();
		InitializeBuffers(mSize);
	}

	mLastX = X;
	mLastY = Y;
	mLastText = const_cast<LPWSTR>(Text.c_str());

	float CX, CY;
	CX = float(m_windowWidth) / 2.0f * -1 + X;
	CY = float(m_windowHeight) / 2.0f - Y;

	D3D11_MAPPED_SUBRESOURCE Vertices, Indices;
	m_d3d11Context->Map(mVertexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Vertices);
	m_d3d11Context->Map(mIndexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Indices);

	mFont->Build(Vertices.pData, Indices.pData, mVertexCount, mIndexCount, Text, CX, CY);

	m_d3d11Context->Unmap(mIndexBuffer.Get(), 0);
	m_d3d11Context->Unmap(mVertexBuffer.Get(), 0);

	Render(cam, Color);
}

void Text::ForceUpdate()
{
	mLastX = -1;
	mLastY = -1;
}

void Text::Render(ICamera * cam, const DirectX::XMFLOAT4& color)
{
	static UINT Stride = sizeof(CFont::SVertex);
	static UINT Offset = 0;
	static auto renderer = Direct3D11::Get();
	static auto bm = BufferManager::Get();

	static auto pipeline = TexturePipeline::Get();
	pipeline->bind(DirectX::XMMatrixIdentity(), cam);
	pipeline->setAdditionalColor(color);

	ID3D11Buffer * vertexBuffers[] = {
		mVertexBuffer.Get()
	};

	m_d3d11Context->IASetVertexBuffers(0, 1, vertexBuffers, &Stride, &Offset);
	m_d3d11Context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	static DirectX::XMMATRIX WVP;
	WVP = cam->GetView() * cam->GetProjection();
	WVP = DirectX::XMMatrixTranspose(WVP);

	ID3D11ShaderResourceView * SRVs[] = 
	{
		mFont->GetTexture()->GetTextureSRV()
	};

	m_d3d11Context->PSSetShaderResources(0, 1, SRVs);
	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSampler.GetAddressOf());
	bm->bindPSBuffer(MATERIAL_SLOT, mMaterialBuffer);

	Direct3D11::Get()->RSCullNone();
	m_d3d11Context->DrawIndexed(mIndexCount, 0, 0);
	g_drawCalls++;
	Direct3D11::Get()->RSLastState();

	pipeline->lastAdditionalColor();
}