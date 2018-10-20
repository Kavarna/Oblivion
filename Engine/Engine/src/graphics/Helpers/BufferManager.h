#pragma once

#include "../../common/interfaces/Singletone.h"
#include "../../common/common.h"
#include "../interfaces/GraphicsObject.h"
#include "ShaderHelper.h"

class BufferManager sealed : public Singletone<BufferManager>, public IGraphicsObject
{
	typedef MicrosoftPointer<ID3D11Buffer> ConstantBuffer;
public:
	BufferManager() = default;
	~BufferManager()
	{
		for (auto & it : m_buffers)
		{
			it.second->Release();
		}
	}

private:
	template <Shader::ShaderType shader>
	inline bool isBufferBound(uint32_t slot, uint32_t handle)
	{
		if constexpr (shader == Shader::ShaderType::eVertex)
			return m_boundVSBuffers[slot] == handle;
		else if constexpr (shader == Shader::ShaderType::eHull)
			return m_boundHSBuffers[slot] == handle;
		else if constexpr (shader == Shader::ShaderType::eDomain)
			return m_boundDSBuffers[slot] == handle;
		else if constexpr (shader == Shader::ShaderType::eGeometry)
			return m_boundGSBuffers[slot] == handle;
		else if constexpr (shader == Shader::ShaderType::ePixel)
			return m_boundPSBuffers[slot] == handle;
		else
			static_assert(false);
	}

public:
	
	inline void bindVSBuffer(uint32_t slot, uint32_t handle)
	{
		if (!isBufferBound<Shader::ShaderType::eVertex>(slot, handle))
		{
			m_d3d11Context->VSSetConstantBuffers(slot, 1, m_buffers[handle].GetAddressOf());
			m_boundVSBuffers[slot] = handle;
		}
	}

	inline void bindVSBuffers(uint32_t startSlot, uint32_t* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].Get();
			m_boundVSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->VSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

private:
	std::array<uint32_t, 16>						m_boundVSBuffers;
	std::array<uint32_t, 16>						m_boundHSBuffers;
	std::array<uint32_t, 16>						m_boundDSBuffers;
	std::array<uint32_t, 16>						m_boundGSBuffers;
	std::array<uint32_t, 16>						m_boundPSBuffers;

	std::unordered_map<uint32_t, ConstantBuffer>	m_buffers;
};