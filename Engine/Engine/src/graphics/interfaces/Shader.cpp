#include "Shader.h"

VertexShaderEnum	IVertexShader::m_boundVertexShader		= VertexShaderNone;
PixelShaderEnum		IPixelShader::m_boundPixelShader		= PixelShaderNone;
DomainShaderEnum	IDomainShader::m_boundDomainShader		= DomainShaderNone;
HullShaderEnum		IHullShader::m_boundHullShader			= HullShaderNone;
GeometryShaderEnum	IGeometryShader::m_boundGeometryShader	= GeometryShaderNone;
ComputeShaderEnum	IComputeShader::m_boundComputeShader	= ComputeShaderNone;

bool IVertexShader::shouldBind(const VertexShaderEnum vs)
{
	if (m_boundVertexShader == vs && vs != VertexShaderEnum::VertexShaderCustom)
		return false;
	return true;
}

IVertexShader::IVertexShader(LPWSTR path)
{
	ID3D11VertexShader ** VS = &m_vertexShader;
	ShaderHelper::CreateShaderFromFile(path, (LPSTR)"vs_4_0",
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

void IVertexShader::unbind() const
{
	if (shouldBind(VertexShaderEnum::VertexShaderNone))
	{
		m_d3d11Context->PSSetShader(nullptr, nullptr, 0);
		m_boundVertexShader = VertexShaderEnum::VertexShaderNone;
	}
}

bool IPixelShader::shouldBind(const PixelShaderEnum ps)
{
	if (m_boundPixelShader == ps && ps != PixelShaderEnum::PixelShaderCustom)
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

void IPixelShader::unbind() const
{
	if (shouldBind(PixelShaderEnum::PixelShaderNone))
	{
		m_d3d11Context->PSSetShader(nullptr, nullptr, 0);
		m_boundPixelShader = PixelShaderEnum::PixelShaderNone;
	}
}

IPixelShader::IPixelShader(LPWSTR path)
{
	ID3D11PixelShader ** PS = &m_pixelShader;
	ShaderHelper::CreateShaderFromFile(path, (LPSTR)"ps_4_0",
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
	if (m_boundDomainShader == ds && ds != DomainShaderEnum::DomainShaderCustom)
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

void IDomainShader::unbind() const
{
	if (shouldBind(DomainShaderEnum::DomainShaderNone))
	{
		m_d3d11Context->DSSetShader(nullptr, nullptr, 0);
		m_boundDomainShader = DomainShaderEnum::DomainShaderNone;
	}
}

IDomainShader::IDomainShader(LPWSTR path)
{
	ID3D11DomainShader ** DS = &m_domainShader;
	ShaderHelper::CreateShaderFromFile(path, (LPSTR)"ds_4_0",
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
	if (m_boundHullShader == hs && hs != HullShaderEnum::HullShaderCustom)
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

void IHullShader::unbind() const
{
	if (shouldBind(HullShaderEnum::HullShaderNone))
	{
		m_d3d11Context->HSSetShader(nullptr, nullptr, 0);
		m_boundHullShader = HullShaderEnum::HullShaderNone;
	}
}

IHullShader::IHullShader(LPWSTR path)
{
	ID3D11HullShader ** HS = &m_hullShader;
	ShaderHelper::CreateShaderFromFile(path, (LPSTR)"hs_4_0",
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

bool IGeometryShader::shouldBind(const GeometryShaderEnum gs)
{
	if (m_boundGeometryShader == gs && gs != GeometryShaderEnum::GeometryShaderCustom)
		return false;
	return true;
}

void IGeometryShader::bind() const
{
	auto shaderType = getGeometryShaderType();
	if (shouldBind(shaderType)) // if current shader is not bound already
	{
		m_d3d11Context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
		m_boundGeometryShader = shaderType;
	}
}

void IGeometryShader::unbind() const
{
	if (shouldBind(GeometryShaderEnum::GeometryShaderNone))
	{
		m_d3d11Context->GSSetShader(nullptr, nullptr, 0);
		m_boundGeometryShader = GeometryShaderEnum::GeometryShaderNone;
	}
}

IGeometryShader::IGeometryShader(LPWSTR path)
{
	ID3D11GeometryShader ** HS = &m_geometryShader;
	ShaderHelper::CreateShaderFromFile(path, (LPSTR)"ds_4_0",
		m_d3d11Device.Get(), &m_shaderBlob,
		reinterpret_cast<ID3D11DeviceChild**>(HS));
}

IGeometryShader::~IGeometryShader()
{
	m_geometryShader.Reset();
}

inline Shader::ShaderType IGeometryShader::getShaderType() const
{
	return Shader::ShaderType::eGeometry;
}

bool IComputeShader::shouldBind(const ComputeShaderEnum cs)
{
	if (m_boundComputeShader == cs && cs != ComputeShaderEnum::ComputeShaderCustom)
		return false;
	return true;
}

void IComputeShader::bind() const
{
	auto shaderType = getComputeShaderType();
	if (shouldBind(shaderType)) // if current shader is not bound already
	{
		m_d3d11Context->CSSetShader(m_computeShader.Get(), nullptr, 0);
		m_boundComputeShader = shaderType;
	}
}

void IComputeShader::unbind() const
{
	if (shouldBind(ComputeShaderEnum::ComputeShaderNone))
	{
		m_d3d11Context->GSSetShader(nullptr, nullptr, 0);
		m_boundComputeShader = ComputeShaderEnum::ComputeShaderNone;
	}
}

IComputeShader::IComputeShader(LPWSTR path)
{
	ID3D11ComputeShader ** CS = &m_computeShader;
	ShaderHelper::CreateShaderFromFile(path, (LPSTR)"cs_5_0", m_d3d11Device.Get(),
		&m_shaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));
}

IComputeShader::~IComputeShader()
{
	m_computeShader.Reset();
}

inline Shader::ShaderType IComputeShader::getShaderType() const
{
	return Shader::ShaderType::eCompute;
}
