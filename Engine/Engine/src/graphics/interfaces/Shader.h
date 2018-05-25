#pragma once

#include "../../common/common.h"
#include "../ShaderHelper.h"

class Direct3D11;

class IShader
{
private:
	friend class Direct3D11;
public:
	struct SVertex; // In this order: Position, Texture, Normals, Tangents, Binormals
public:
	IShader(uint64_t code) : m_shaderCode(code) {};
	virtual ~IShader() {};

public:
	struct SCameraInformations
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};
	struct SMaterialInformations
	{
		// To be added
	};

private:
	virtual void		bind(ID3D11DeviceContext*) const	= 0;
	virtual void		Create(ID3D11Device*)				= 0;
	virtual bool		additionalData() const				= 0;
	virtual void		bindCameraInformations(ID3D11DeviceContext*, SCameraInformations const*) const
	{
		return;
	}
	virtual void		bindMaterialInformations(ID3D11DeviceContext*, SMaterialInformations const*) const
	{
		return;
	}

private:
	uint64_t			m_shaderCode; // To remember which shader is bound to the pipeline
};