#pragma once


#include "../../common/common.h"
#include "Camera.h"
#include "AlignedObject.h"
#include "Shader.h"


template <class shader>
class IGameObject : public AlignedObject
{
	static_assert(std::is_base_of<IShader, shader>::value,
		"Can't create a game object with a non-shader generic argument");
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
	virtual void Render(ICamera *, int) const = 0;
	virtual void Update(float frameTime) = 0;
	virtual void Destroy() = 0;



protected:
	virtual uint32_t GetIndexCount() const = 0;
	virtual uint32_t GetVertexCount() const = 0;

protected:
	MicrosoftPointer<ID3D11Device>			m_d3d11Device;
	MicrosoftPointer<ID3D11DeviceContext>	m_d3d11Context;
	DirectX::XMMATRIX						m_objectWorld;
};