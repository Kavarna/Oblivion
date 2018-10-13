#include "Shader.h"

VertexShaderEnum	IVertexShader::m_boundVertexShader	= VertexShaderNone;
PixelShaderEnum		IPixelShader::m_boundPixelShader	= PixelShaderNone;
DomainShaderEnum	IDomainShader::m_boundDomainShader	= DomainShaderNone;
HullShaderEnum		IHullShader::m_boundHullShader		= HullShaderNone;

bool IVertexShader::shouldBind(const VertexShaderEnum vs)
{
	if (m_boundVertexShader == vs)
		return false;
	return true;
}

IVertexShader::IVertexShader(LPWSTR path)
{
	ID3D11VertexShader ** VS = &m_vertexShader;
	ShaderHelper::CreateShaderFromFile(path, "vs_4_0",
		m_d3d11Device.Get(), &m_shaderBlob,
		reinterpret_cast<ID3D11DeviceChild**>(VS));
}

IVertexShader::~IVertexShader()
{
	m_vertexShader.Reset();

	m_inputLayout.Reset();
}

Shader::ShaderType IVertexShader::getShaderType() const
{
	return Shader::ShaderType::eVertex;
}

void IVertexShader::bind() const
{
	auto shaderType = getVertexShaderType();
	if (shouldBind(shaderType)) // if current shader is not bound already
	{
		m_d3d11Context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		m_d3d11Context->IASetInputLayout(m_inputLayout.Get());
		m_boundVertexShader = shaderType;
	}
}

bool IPixelShader::shouldBind(const PixelShaderEnum ps)
{
	if (m_boundPixelShader == ps)
		return false;
	return true;
}

void IPixelShader::bind() const
{
	auto shaderType = getPixelShaderType();
	if (shouldBind(shaderType)) // if current shader is not bound already
	{
		m_d3d11Context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
		m_boundPixelShader = shaderType;
	}
}

IPixelShader::IPixelShader(LPWSTR path)
{
	ID3D11PixelShader ** PS = &m_pixelShader;
	ShaderHelper::CreateShaderFromFile(path, "ps_4_0",
		m_d3d11Device.Get(), &m_shaderBlob,
		reinterpret_cast<ID3D11DeviceChild**>(PS));
}

IPixelShader::~IPixelShader()
{
	m_pixelShader.Reset();
}

inline Shader::ShaderType IPixelShader::getShaderType() const
{
	return Shader::ShaderType::ePixel;
}

bool IDomainShader::shouldBind(const DomainShaderEnum ds)
{
	if (m_boundDomainShader == ds)
		return false;
	return true;
}

void IDomainShader::bind() const
{
	auto shaderType = getDomainShaderType();
	if (shouldBind(shaderType)) // if current shader is not bound already
	{
		m_d3d11Context->DSSetShader(m_domainShader.Get(), nullptr, 0);
		m_boundDomainShader = shaderType;
	}
}

IDomainShader::IDomainShader(LPWSTR path)
{
	ID3D11DomainShader ** DS = &m_domainShader;
	ShaderHelper::CreateShaderFromFile(path, "ds_4_0",
		m_d3d11Device.Get(), &m_shaderBlob,
		reinterpret_cast<ID3D11DeviceChild**>(DS));
}

IDomainShader::~IDomainShader()
{
	m_domainShader.Reset();
}

inline Shader::ShaderType IDomainShader::getShaderType() const
{
	return Shader::ShaderType::eDomain;
}

bool IHullShader::shouldBind(const HullShaderEnum hs)
{
	if (m_boundHullShader == hs)
		return false;
	return true;
}

void IHullShader::bind() const
{
	auto shaderType = getHullShaderType();
	if (shouldBind(shaderType)) // if current shader is not bound already
	{
		m_d3d11Context->HSSetShader(m_hullShader.Get(), nullptr, 0);
		m_boundHullShader = shaderType;
	}
}

IHullShader::IHullShader(LPWSTR path)
{
	ID3D11HullShader ** HS = &m_hullShader;
	ShaderHelper::CreateShaderFromFile(path, "ds_4_0",
		m_d3d11Device.Get(), &m_shaderBlob,
		reinterpret_cast<ID3D11DeviceChild**>(HS));
}

IHullShader::~IHullShader()
{
	m_hullShader.Reset();
}

inline Shader::ShaderType IHullShader::getShaderType() const
{
	return Shader::ShaderType::eHull;
}
