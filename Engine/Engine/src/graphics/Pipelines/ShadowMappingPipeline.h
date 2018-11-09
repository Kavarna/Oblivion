#pragma once

#include "Pipeline.h"
#include "../Shaders/SimpleVertexShader.h"
#include "../Shaders/InstancedVertexShader.h"
#include "../Shaders/DisplacementMappingHullShader.h"
#include "../Shaders/DisplacementMappingDomainShader.h"
#include "../Shaders/ShadowMappingPixelShader.h"
#include "TextureLightPipeline.h"
#include "DisplacementLightPipeline.h"
#include "../Texture.h"
#include "../Helpers/Lights.h"


class ShadowMappingPipeline : public IPipeline, public Singletone<ShadowMappingPipeline>,
	public IDynamicDisplacementPipeline
{
public:
	ShadowMappingPipeline()
	{
		m_usedStages = (uint32_t)Shader::ShaderType::ePixel |
			(uint32_t)Shader::ShaderType::eVertex |
			(uint32_t)Shader::ShaderType::eHull |
			(uint32_t)Shader::ShaderType::eDomain;
	}

//	void setShadowMap(const Shadowmap *);
//private:
//	const Shadowmap * m_shadowmap;
	
	void setShadowMap(const Texture* tex, DirectionalLightView* directionalLight);

private:
	const Texture*					m_shadowmapTexture;
	DirectionalLightView*			m_directionalLight;


public:

	// Inherited via IPipeline
	virtual void __vectorcall bind(ICamera * cam) const override;

	virtual void __vectorcall bind(DirectX::FXMMATRIX & world, ICamera * cam) const override;

};