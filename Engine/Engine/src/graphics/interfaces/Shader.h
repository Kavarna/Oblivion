#pragma once

#include "../../common/common.h"
#include "../../common/interfaces/Singletone.h"
#include "../interfaces/GraphicsObject.h"
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

class IShader : public IGraphicsObject
{
public:
	virtual void							bind() const = 0;
	virtual void							unbind() const = 0;

	virtual inline Shader::ShaderType		getShaderType() const = 0;

	inline	LPVOID							getShaderBlobPointer() const { return m_shaderBlob->GetBufferPointer(); };
	inline	SIZE_T							getShaderBlobSize() const { return m_shaderBlob->GetBufferSize(); };

protected:
	MicrosoftPointer<ID3DBlob>				m_shaderBlob;
	
};

enum VertexShaderEnum
{
	VertexShaderNone = 0,
	VertexShaderSimple,
	VertexShaderInstanced,
	VertexShaderCustom = ~0
};

class IVertexShader : public IShader
{
private:
	static VertexShaderEnum					m_boundVertexShader;

public:
	static bool								shouldBind(const VertexShaderEnum);
	virtual void							bind() const;
	virtual void							unbind() const;
public:
	IVertexShader() = default;
	IVertexShader(LPWSTR path);
	virtual ~IVertexShader();

public:
	virtual void							createInputLayout() = 0;

public:
	virtual VertexShaderEnum				getVertexShaderType() const = 0;
	virtual inline Shader::ShaderType		getShaderType() const final;
	
protected:
	MicrosoftPointer<ID3D11InputLayout>		m_inputLayout	= nullptr;

private:
	MicrosoftPointer<ID3D11VertexShader>	m_vertexShader	= nullptr;

};

enum PixelShaderEnum
{
	PixelShaderNone = 0,
	PixelShaderBasic,
	PixelShaderBatch,
	PixelShaderDisplacement,
	PixelShaderTexture,
	PixelShaderTextureLight,
	PixelShaderCustom = ~0
};

class IPixelShader : public IShader
{
private:
	static PixelShaderEnum					m_boundPixelShader;

public:
	static bool								shouldBind(const PixelShaderEnum);
	virtual void							bind() const;
	virtual void							unbind() const;
public:
	IPixelShader() = default;
	IPixelShader(LPWSTR path);
	virtual ~IPixelShader();

public:
	virtual PixelShaderEnum					getPixelShaderType() const = 0;
	virtual inline Shader::ShaderType		getShaderType() const final;

private:
	MicrosoftPointer<ID3D11PixelShader>		m_pixelShader	= nullptr;
};

enum DomainShaderEnum
{
	DomainShaderNone = 0,
	DomainShaderDisplacement,
	DomainShaderCustom = ~0
};

class IDomainShader : public IShader
{
private:
	static DomainShaderEnum					m_boundDomainShader;

public:
	static bool								shouldBind(const DomainShaderEnum);
	virtual void							bind() const;
	virtual void							unbind() const;
public:
	IDomainShader() = default;
	IDomainShader(LPWSTR path);
	virtual ~IDomainShader();

public:
	virtual DomainShaderEnum				getDomainShaderType() const = 0;
	virtual inline Shader::ShaderType		getShaderType() const final;

private:
	MicrosoftPointer<ID3D11DomainShader>	m_domainShader	= nullptr;
};

enum HullShaderEnum
{
	HullShaderNone = 0,
	HullShaderDisplacement,
	HullShaderCustom = ~0
};

class IHullShader : public IShader
{
private:
	static HullShaderEnum					m_boundHullShader;

public:
	static bool								shouldBind(const HullShaderEnum);
	virtual void							bind() const;
	virtual void							unbind() const;
public:
	IHullShader() = default;
	IHullShader(LPWSTR path);
	virtual ~IHullShader();

public:
	virtual HullShaderEnum					getHullShaderType() const = 0;
	virtual inline Shader::ShaderType		getShaderType() const final;

private:
	MicrosoftPointer<ID3D11HullShader>		m_hullShader	= nullptr;
};

enum GeometryShaderEnum
{
	GeometryShaderNone = 0,
	GeometryShaderCustom = ~0
};

class IGeometryShader : public IShader
{
private:
	static GeometryShaderEnum				m_boundGeometryShader;

public:
	static bool								shouldBind(const GeometryShaderEnum);
	virtual void							bind() const;
	virtual void							unbind() const;
public:
	IGeometryShader() = default;
	IGeometryShader(LPWSTR path);
	virtual ~IGeometryShader();

public:
	virtual GeometryShaderEnum					getGeometryShaderType() const = 0;
	virtual inline Shader::ShaderType		getShaderType() const final;

private:
	MicrosoftPointer<ID3D11GeometryShader>	m_geometryShader = nullptr;
};