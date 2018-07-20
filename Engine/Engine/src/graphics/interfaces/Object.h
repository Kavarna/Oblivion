#pragma once

#include "../Direct3D11.h"
#include "../../common/Globals.h"

class IObject
{
public:
	IObject()
	{
		auto renderer = Direct3D11::Get();
		m_d3d11Device = renderer->getDevice();
		m_d3d11Context = renderer->getContext();
	}
	virtual ~IObject()
	{
		m_d3d11Device.Reset();
		m_d3d11Context.Reset();
	}

	void setContext(MicrosoftPointer<ID3D11DeviceContext> ctx)
	{
		m_d3d11Context.Reset();
		m_d3d11Context = ctx;
	};

	void setDevice(MicrosoftPointer<ID3D11Device> dev)
	{
		m_d3d11Device.Reset();
		m_d3d11Device = dev;
	}

	MicrosoftPointer<ID3D11Device>	getDevice() const
	{
		return m_d3d11Device;
	}

	MicrosoftPointer<ID3D11DeviceContext> getContext() const
	{
		return m_d3d11Context;
	}

protected:
	MicrosoftPointer<ID3D11Device>			m_d3d11Device;
	MicrosoftPointer<ID3D11DeviceContext>	m_d3d11Context;

};