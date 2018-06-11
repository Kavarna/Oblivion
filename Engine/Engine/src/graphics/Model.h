#pragma once


#include "interfaces/GameObject.h"
#include "Direct3D11.h"
#include "Shaders/BasicShader.h"
#include "Shaders/TextureLightShader.h"

enum class EDefaultObject
{
	Box, Sphere, Geosphere, Cylinder, Grid, FullscreenQuad
};

class Model : public IGameObject
{
public:
	struct SVertex
	{
		SVertex() = default;
		SVertex(float x, float y, float z,
			float tu, float tv,
			float nx, float ny, float nz) :
			Position(x, y, z), TexC(tu, tv),
			Normal(nx, ny, nz) {};
		SVertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv) :
			Position(p), Normal(n), TangentU(t), TexC(uv)
		{
			DirectX::XMVECTOR Normal, Tangent;
			Normal = DirectX::XMLoadFloat3(&this->Normal);
			Tangent = DirectX::XMLoadFloat3(&this->TangentU);
			DirectX::XMStoreFloat3(&this->Binormal, DirectX::XMVector3Cross(Normal, Tangent));
		}
		SVertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v)
		{
			DirectX::XMVECTOR Normal, Tangent;
			Normal = DirectX::XMLoadFloat3(&this->Normal);
			Tangent = DirectX::XMLoadFloat3(&this->TangentU);
			DirectX::XMStoreFloat3(&this->Binormal, DirectX::XMVector3Cross(Normal, Tangent));
		}

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 TexC;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 TangentU;
		DirectX::XMFLOAT3 Binormal;
	};
public:
	Model();
	~Model();

public:
	void Update(float frameTime)
	{
	};

public:
	void Create(LPWSTR lpPath);
	void Create(EDefaultObject object);
	template <class Shader, bool bindShader = false>
	void Render(ICamera * cam, int instanceCount = 1) const;
	void Destroy();

public:
	inline uint32_t GetIndexCount() const;
	inline uint32_t GetVertexCount() const;


private:
	MicrosoftPointer<ID3D11Buffer>	m_vertexBuffer;
	MicrosoftPointer<ID3D11Buffer>	m_indexBuffer;

	std::unique_ptr<Texture>		m_texture;
	
	std::vector<uint32_t>			m_startIndices;
	std::vector<SVertex>			m_vertices;
	std::vector<uint32_t>			m_indices;
};


template <class Shader, bool bindShader>
void Model::Render(ICamera * cam, int instanceCount) const
{
	static_assert(std::is_base_of<IShader, Shader>::value,
		"Can't render a game object with a non-shader generic argument");
	if constexpr (bindShader)
	{
		Shader::Get()->bind();
	}
	if constexpr (std::is_same<Shader,BasicShader>::value)
	{
		BasicShader * shader = Shader::Get();
		static DirectX::XMMATRIX WVP;
		WVP = m_objectWorld * cam->GetView() * cam->GetProjection();
		WVP = DirectX::XMMatrixTranspose(WVP);
		shader->SetCameraInformations({ WVP });
	}
	else if constexpr (std::is_same<Shader, TextureLightShader>::value)
	{
		auto renderer = Direct3D11::Get();
		TextureLightShader * shader = TextureLightShader::Get();
		static DirectX::XMMATRIX WVP;
		WVP = m_objectWorld * cam->GetView() * cam->GetProjection();
		WVP = DirectX::XMMatrixTranspose(WVP);
		shader->SetCameraInformations({
			DirectX::XMMatrixTranspose(m_objectWorld),
			DirectX::XMMatrixTranspose(cam->GetView()),
			DirectX::XMMatrixTranspose(cam->GetProjection())
			});

		ID3D11ShaderResourceView * textures[1] = 
		{
			m_texture->GetTextureSRV()
		};
		m_d3d11Context->PSSetShaderResources(0, 1, textures);
		m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSamplerState.GetAddressOf());
	}
	else
	{
		// TODO: Add message with shader type
	}

	if (instanceCount != 0)
		DX::OutputVDebugString(L"Unable to draw instanced for moment.\n");

	uint32_t offset = 0;
	uint32_t stride = sizeof(SVertex);
	assert(stride >= 0);
	m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11Context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	m_d3d11Context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_d3d11Context->DrawIndexed(GetIndexCount(), 0, 0);
}