#pragma once

#include "../../common/common.h"
#include "../ShaderHelper.h"

class Direct3D11;

class IShader
{
private:
	friend class Direct3D11;
	struct SShaderCode
	{
		uint16_t m_vertexShader;
		uint16_t m_hullShader;
		uint16_t m_domainShader;
		uint16_t m_geometryShader;
		uint16_t m_pixelShader;
	};
public:
	struct SVertex; // In this order: Position, Texture, Normals, Tangents, Binormals
public:
	IShader(SShaderCode code) : m_shaderCode(code) {};
	virtual ~IShader() {};

private:
	virtual void		bind(ID3D11DeviceContext*) const	= 0;
	virtual void		Create(ID3D11Device*)				= 0;

private:
	SShaderCode			m_shaderCode; // To remember which shader is bound to the pipeline
};