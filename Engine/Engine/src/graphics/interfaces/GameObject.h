#pragma once


#include "../../common/common.h"
#include "Camera.h"
#include "AlignedObject.h"
#include "Shader.h"

class Direct3D11;

template <class Shader>
class IGameObject : public AlignedObject
{
	friend class Direct3D11;
public:
	IGameObject() {};
	virtual ~IGameObject() {};

public:

	virtual void Update(float frameTime) = 0;

protected:
	virtual void Create(Shader*, ID3D11Device*, ID3D11DeviceContext*) = 0;
	virtual void Render(ID3D11DeviceContext*) = 0;
	virtual void Destroy() = 0;
	virtual uint32_t GetIndexCount() const = 0;
	virtual uint32_t GetVertexCount() const = 0;

protected:
	virtual uint32_t GetStartIndexLocation()
	{
		return m_startIndexLocation;
	};
	virtual uint32_t GetStartVertexLocation()
	{
		return m_startVertexLocation;
	};

protected:

	IShader* m_shader;

	uint32_t m_startIndexLocation = 0;
	uint32_t m_startVertexLocation = 0;
};