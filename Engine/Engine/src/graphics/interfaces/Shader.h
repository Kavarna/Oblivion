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
		float tessMin; // 4
		float tessMax; // 4
		float tessScale;
		float pad; // 4
	} SMaterial, Material;

}

enum Pipeline
{
	PipelineNone = 0,
	PipelineBatchShader = -1,
	PipelineTextureBatchShader = -2,
	PipelineBasic = 1,
	PipelineTexture = 2,
	PipelineTextureLight = 3,
	PipelineDisplacementTextureLight = 4,
};

class IShader : public IObject
{
public:
	struct SVertex; // For consistency
protected:
	IShader() {};
	virtual ~IShader() {};

private:
	static Pipeline							m_currentlyBoundPipeline;

protected:
	static bool								shouldBind(const Pipeline& p)
	{
		if (p == m_currentlyBoundPipeline)
			return false;
		return true;
	}

protected:
	MicrosoftPointer<ID3D11VertexShader>	m_vertexShader;
	MicrosoftPointer<ID3D11HullShader>		m_hullShader;
	MicrosoftPointer<ID3D11DomainShader>	m_domainShader;
	MicrosoftPointer<ID3D11GeometryShader>	m_geometryShader;
	MicrosoftPointer<ID3D11PixelShader>		m_pixelShader;
	MicrosoftPointer<ID3D11ComputeShader>	m_computeShader;
	MicrosoftPointer<ID3D11InputLayout>		m_inputLayout;

protected:
	virtual const Pipeline					GetPipelineType() const = 0;

public:
	virtual void							Create()		= 0;
	void									bind() const
	{
		auto p = GetPipelineType();
		if (shouldBind(p))
		{
			m_currentlyBoundPipeline = p;

			m_d3d11Context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
			m_d3d11Context->IASetInputLayout(m_inputLayout.Get());
			m_d3d11Context->HSSetShader(m_hullShader.Get(), nullptr, 0);
			m_d3d11Context->DSSetShader(m_domainShader.Get(), nullptr, 0);
			m_d3d11Context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
			m_d3d11Context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
			m_d3d11Context->CSSetShader(m_computeShader.Get(), nullptr, 0);
		}
	}
	
	static void								LuaRegister();
	static IShader*							Get()
	{
		return nullptr;
	}

};