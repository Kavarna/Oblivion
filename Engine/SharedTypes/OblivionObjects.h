#pragma once

#include <DirectXMath.h>
#include <string>
#include <cstdlib>
#include <cstdarg>

#define ZeroMemoryAndDeclare(type, name) type name;\
ZeroMemory(&name,sizeof(type));
#define ZeroVariable(name) ZeroMemory(&name,sizeof(decltype(name)))
#define ALIGN16 __declspec(align(16))

#define THROW_ERROR(...) {char message[1024];\
sprintf_s(message,sizeof(message),__VA_ARGS__);\
std::exception error(message);\
throw error;}\

#define MAKE_SINGLETONE(singletoneName) private:\
static std::once_flag						m_singletoneFlag;\
static std::unique_ptr<singletoneName>		m_singletoneInstance;\
public:\
static singletoneName* Get();
#define DECLARE_SINGLETONE(singletoneName) std::once_flag	singletoneName::m_singletoneFlag;\
std::unique_ptr<singletoneName>	singletoneName::m_singletoneInstance;\
singletoneName * singletoneName::Get()\
{\
std::call_once(m_singletoneFlag, [&] { m_singletoneInstance = std::make_unique<singletoneName>(); m_singletoneInstance->Create(); });\
return m_singletoneInstance.get();\
}
#define DECLARE_SINGLETONE_WITHOUT_CREATE(singletoneName) std::once_flag	singletoneName::m_singletoneFlag;\
std::unique_ptr<singletoneName>	singletoneName::m_singletoneInstance;\
singletoneName * singletoneName::Get()\
{\
std::call_once(m_singletoneFlag, [&] { m_singletoneInstance = std::make_unique<singletoneName>(); });\
return m_singletoneInstance.get();\
}



namespace Oblivion
{
	typedef struct material_t
	{
		material_t() = default;
		std::string name;
		DirectX::XMFLOAT4 diffuseColor; // r, g, b, a
		float specularPower;
		std::string specularMap;
		std::string diffuseMap;
		std::string bumpMap;

	} Material, SMaterial;

	typedef struct mesh_t
	{

		mesh_t() = default;
		uint32_t numIndices;
		uint32_t baseVertex;
		uint32_t baseIndex;
		uint32_t materialIndex;

	} Mesh, SMesh;

	typedef struct vertex_t
	{
		vertex_t() = default;
		vertex_t(float x, float y, float z,
			float tu, float tv,
			float nx, float ny, float nz) :
			Position(x, y, z), TexC(tu, tv),
			Normal(nx, ny, nz) {};
		vertex_t(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT3& t, const DirectX::XMFLOAT2& uv) :
			Position(p), Normal(n), TangentU(t), TexC(uv)
		{
			DirectX::XMVECTOR Normal, Tangent;
			Normal = DirectX::XMLoadFloat3(&this->Normal);
			Tangent = DirectX::XMLoadFloat3(&this->TangentU);
			DirectX::XMStoreFloat3(&this->Binormal, DirectX::XMVector3Cross(Normal, Tangent));
		}
		vertex_t(
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
	} Vertex, SVertex;
}