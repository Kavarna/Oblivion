#pragma once

#include "../../common/common.h"
#include "../Helpers/ShaderHelper.h"

class Direct3D11;

class IShader
{
public:
	struct SVertex; // For consistency
protected:
	IShader() {};
	virtual ~IShader()
	{
		m_d3d11Device.Reset();
		m_d3d11Context.Reset();
	};

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

protected:
	MicrosoftPointer<ID3D11Device>			m_d3d11Device;
	MicrosoftPointer<ID3D11DeviceContext>	m_d3d11Context;
};

