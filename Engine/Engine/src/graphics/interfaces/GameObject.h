#pragma once


#include "../../common/common.h"
#include "../Texture.h"
#include "Camera.h"
#include "AlignedObject.h"
#include "Shader.h"



class IGameObject : public AlignedObject
{
	friend class Direct3D11;
public:
	IGameObject() {};
	virtual ~IGameObject() 
	{
		m_d3d11Device.Reset();
		m_d3d11Context.Reset();
	};

public:
	virtual void Create(LPWSTR lpPath) = 0;
	//virtual void Render(ICamera *, int) const = 0; /// Should be templated
	virtual void Update(float frameTime) = 0;
	virtual void Destroy() = 0;

public:
	inline void Identity() { m_objectWorld = DirectX::XMMatrixIdentity(); };
	inline void Scale(float S) { m_objectWorld *= DirectX::XMMatrixScaling(S, S, S); };
	inline void Translate(float x, float y, float z) { m_objectWorld *= DirectX::XMMatrixTranslation(x, y, z); };
	inline void RotateX(float Theta) { m_objectWorld *= DirectX::XMMatrixRotationX(Theta); };
	inline void RotateY(float Theta) { m_objectWorld *= DirectX::XMMatrixRotationY(Theta); };
	inline void RotateZ(float Theta) { m_objectWorld *= DirectX::XMMatrixRotationZ(Theta); };

protected:
	virtual uint32_t GetIndexCount() const = 0;
	virtual uint32_t GetVertexCount() const = 0;

protected:
	MicrosoftPointer<ID3D11Device>			m_d3d11Device;
	MicrosoftPointer<ID3D11DeviceContext>	m_d3d11Context;
	DirectX::XMMATRIX						m_objectWorld;
};