#pragma once


#include "../../common/common.h"
#include "../../common/commontypes.h"
#include "../Texture.h"
#include "Camera.h"
#include "AlignedObject.h"
#include "Shader.h"



class IGameObject : public AlignedObject
{
	friend class Direct3D11;
public:
	IGameObject() = default;
	virtual ~IGameObject() 
	{
		m_d3d11Device.Reset();
		m_d3d11Context.Reset();
	};

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
	virtual void Create(LPWSTR lpPath) = 0;
	virtual void Update(float frameTime) = 0;
	virtual void Destroy() = 0;

public:
	inline void Identity(int instanceID = 0) { m_objectWorld[instanceID] = DirectX::XMMatrixIdentity(); };
	inline void Scale(float S, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixScaling(S, S, S); };
	inline void Translate(float x, float y, float z, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixTranslation(x, y, z); };
	inline void RotateX(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationX(Theta); };
	inline void RotateY(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationY(Theta); };
	inline void RotateZ(float Theta, int instanceID = 0) { m_objectWorld[instanceID] *= DirectX::XMMatrixRotationZ(Theta); };

protected:
	virtual uint32_t GetIndexCount(int subObject = 0) const = 0;
	virtual uint32_t GetVertexCount() const = 0;

public:
	/*
	* returns the index of the new added instance
	*/
	virtual DirectX::XMMATRIX&				AddInstance(DirectX::FXMMATRIX const& mat = DirectX::XMMatrixIdentity());
	virtual DirectX::XMMATRIX*				AddInstance(uint32_t number);
	virtual void							RemoveInstance(int ID);
	virtual void							RemoveInstance(CommonTypes::Range const& range);

protected:
	/*
	* Range between start index and end index
	*/
	static CommonTypes::Range				AddVertices(std::vector<SVertex> & vertices);
	static void								RemoveVertices(CommonTypes::Range const&);
	
public:
	static void								BindStaticVertexBuffer();

protected:
	static std::vector<SVertex>				m_staticVertices;
	static MicrosoftPointer<ID3D11Buffer>	m_staticVerticesBuffer;

protected:
	MicrosoftPointer<ID3D11Device>			m_d3d11Device;
	MicrosoftPointer<ID3D11DeviceContext>	m_d3d11Context;
	MicrosoftPointer<ID3D11Buffer>			m_instanceBuffer;
	std::vector<DirectX::XMMATRIX>			m_objectWorld;
};