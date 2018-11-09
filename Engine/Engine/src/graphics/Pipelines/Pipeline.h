#pragma once

#include "../../common/interfaces/Singletone.h"
#include "../interfaces/ICamera.h"
#include "../Shaders/EmptyShader.h"
#include "../Direct3D11.h"

enum PipelineEnum
{
	PipelineNone = 0,
	PipelineBatchShader = -1,
	PipelineTextureBatchShader = -2,
	PipelineSimpleTexture = -3,
	PipelineBasic = 1,
	PipelineTexture = 2,
	PipelineTextureLight = 3,
	PipelineDisplacementTextureLight = 4,
	PipelineCompute2D = 5,
	PipelineDepthmap = 6,
	PipelineShadowMapping = 7,
	PipelineCustom = INT_MAX
};

class IPipeline : public IGraphicsObject
{
public:
	virtual void __vectorcall	bind(ICamera * cam) const = 0;
	virtual void __vectorcall	bind(DirectX::FXMMATRIX& world, ICamera * cam) const = 0;

	virtual void				clearPipeline() const
	{
		if (!(m_usedStages & (uint32_t)Shader::ShaderType::eVertex))
			EmptyVertexShader::Get()->bind();
		if (!(m_usedStages & (uint32_t)Shader::ShaderType::eDomain))
			EmptyDomainShader::Get()->bind();
		if (!(m_usedStages & (uint32_t)Shader::ShaderType::eHull))
			EmptyHullShader::Get()->bind();
		if (!(m_usedStages & (uint32_t)Shader::ShaderType::ePixel))
			EmptyPixelShader::Get()->bind();
		if (!(m_usedStages & (uint32_t)Shader::ShaderType::eGeometry))
			EmptyGeometryShader::Get()->bind();
	};
public:
	uint32_t					m_usedStages = 0;

};

class IDynamicDisplacementPipeline
{
private:
	bool m_useDisplacement = false;

public:

	void ToggleDisplacement() { m_useDisplacement = !m_useDisplacement; };
	void EnableDisplacement() { m_useDisplacement = true; };
	void DisableDisplacement() { m_useDisplacement = false; };

	bool useDisplacement() const { return m_useDisplacement; };
};