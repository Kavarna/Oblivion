#pragma once

#include "../../common/common.h"
#include "../interfaces/Object.h"
#include "../Helpers/ShaderHelper.h"
#include "../Direct3D11.h"

namespace Shader
{
	typedef struct material_t
	{
		DirectX::XMFLOAT4 color; // 16
		float specular; // 4
		BOOL hasTexture; // 4
		BOOL hasBump; // 4
		BOOL hasSpecular; // 4
	} SMaterial, Material;

}

class IShader : public IObject
{
public:
	struct SVertex; // For consistency
protected:
	IShader() {};
	virtual ~IShader() {};

private:
	static std::type_index					m_currentlyBoundShader;

protected:
	template <class Shader>
	static bool								shouldBind()
	{
		// Gets the shader type and checks if it's already 
		std::type_index shaderType(typeid(Shader));
		if (shaderType == m_currentlyBoundShader)
			return false;
		m_currentlyBoundShader = shaderType;
		return true;
	};

public:
	virtual void							bind() const	= 0;
	virtual void							Create()		= 0;
	
	static IShader*							Get()
	{
		return nullptr;
	}

};