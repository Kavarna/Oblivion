#pragma once

#include "../../common/interfaces/Singletone.h"
#include "../../common/common.h"
#include "../interfaces/GraphicsObject.h"
#include "ShaderHelper.h"

constexpr auto MATERIAL_SLOT = 2;

typedef uint32_t ConstantBufferHandle;

class BufferManager sealed : public Singletone<BufferManager>, public IGraphicsObject
{
	//typedef MicrosoftPointer<ID3D11Buffer> ConstantBuffer;
	struct ConstantBuffer
	{
		MicrosoftPointer<ID3D11Buffer> buffer;
		D3D11_USAGE usage;
		D3D11_MAP mapFlags;
		int CPUAccessFlags;
		uint32_t size;
	};

public:
	BufferManager()
	{
		m_buffers.emplace_back();
	};
	~BufferManager() = default;

private:
	template <Shader::ShaderType shader>
	inline bool isBufferBound(uint32_t slot, ConstantBufferHandle handle)
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
		else if constexpr (shader == Shader::ShaderType::eCompute)
			return m_boundCSBuffers[slot] == handle;
		else
			static_assert(false);
	}

public:

	inline void* MapBuffer(ConstantBufferHandle handle, uint16_t subResource = 0)
	{
		if (m_buffers[handle].CPUAccessFlags == 0)
			return nullptr;
		static D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ThrowIfFailed(
			m_d3d11Context->Map(m_buffers[handle].buffer.Get(),
				subResource, m_buffers[handle].mapFlags, 0, &mappedSubresource)
		);
		return mappedSubresource.pData;
	}

	inline void UnmapBuffer(ConstantBufferHandle handle,
		uint16_t subResource = 0)
	{
		m_d3d11Context->Unmap(m_buffers[handle].buffer.Get(), subResource);
	}

	inline void MapBuffer(ConstantBufferHandle handle,
		void * data,
		uint16_t subresource = 0)
	{
		auto buffMem = MapBuffer(handle, subresource);
		if (buffMem != nullptr)
		{
			memcpy_s(buffMem, m_buffers[handle].size, data, m_buffers[handle].size);
			UnmapBuffer(handle, subresource);
		}
		else
		{
			/*m_d3d11Context->UpdateSubresource(m_buffers[handle].buffer.Get(),
				subresource, nullptr, &data, 0, 0);*/
			m_d3d11Context->UpdateSubresource(m_buffers[handle].buffer.Get(),
				subresource, nullptr, data, 0, 0);
		}
	}
	
	inline ConstantBufferHandle CreateBuffer(D3D11_USAGE Usage, size_t DataSize, int CPUAccessFlag,
		void* data = nullptr, UINT MiscFlag = 0, UINT StructureByteStride = 0)
	{
		MicrosoftPointer<ID3D11Buffer> buffer;
		ZeroMemoryAndDeclare(D3D11_BUFFER_DESC, buffDesc);
		ZeroMemoryAndDeclare(D3D11_SUBRESOURCE_DATA, buffData);
		buffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		buffDesc.ByteWidth = (UINT)DataSize;
		buffDesc.CPUAccessFlags = CPUAccessFlag;
		buffDesc.MiscFlags = MiscFlag;
		buffDesc.StructureByteStride = StructureByteStride;
		buffDesc.Usage = Usage;
		if (data /*!= nullptr*/)
		{
			buffData.pSysMem = data;
			ThrowIfFailed(
				m_d3d11Device->CreateBuffer(&buffDesc, &buffData, &buffer)
				);
		}
		else
		{
			ThrowIfFailed(
				m_d3d11Device->CreateBuffer(&buffDesc, nullptr, &buffer)
				);
		}
		D3D11_MAP mapType;
		if (Usage == D3D11_USAGE::D3D11_USAGE_DEFAULT || Usage == D3D11_USAGE::D3D11_USAGE_IMMUTABLE)
			mapType = D3D11_MAP::D3D11_MAP_READ_WRITE; // Randomly chosen
		if (Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		{
			if ((CPUAccessFlag & D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE) &&
				(CPUAccessFlag & D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ))
				mapType = D3D11_MAP::D3D11_MAP_READ_WRITE;
			else if (CPUAccessFlag == D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ)
				mapType = D3D11_MAP::D3D11_MAP_READ;
			else if (CPUAccessFlag == D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE)
				mapType = D3D11_MAP::D3D11_MAP_WRITE_DISCARD;
		}
		m_buffers.push_back({ buffer, Usage, mapType, CPUAccessFlag, DataSize });
		return (ConstantBufferHandle)m_buffers.size() - 1;
	}

	template <Shader::ShaderType shader>
	inline void bindBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if constexpr (shader == Shader::ShaderType::eVertex)
			bindVSBuffer(slot, handle);
		else if constexpr (shader == Shader::ShaderType::eHull)
			bindHSBuffer(slot, handle);
		else if constexpr (shader == Shader::ShaderType::eDomain)
			bindDSBuffer(slot, handle);
		else if constexpr (shader == Shader::ShaderType::eGeometry)
			bindGSBuffer(slot, handle);
		else if constexpr (shader == Shader::ShaderType::ePixel)
			bindPSBuffer(slot, handle);
		else if constexpr (shader == Shader::ShaderType::eCompute)
			bindCSBuffer(slot, handle);
		else
			static_assert(false);
	}

	inline void bindBuffers(uint32_t shader, uint32_t slot, ConstantBufferHandle* handles, uint32_t num)
	{
		if (shader & (uint32_t)Shader::ShaderType::eVertex)
		{
			if (num == 1)
				bindVSBuffer(slot, handles[0]);
			else
				bindVSBuffers(slot, handles, num);
		}
		if (shader & (uint32_t)Shader::ShaderType::eHull)
		{
			if (num == 1)
				bindHSBuffer(slot, handles[0]);
			else
				bindHSBuffers(slot, handles, num);
		}
		if (shader & (uint32_t)Shader::ShaderType::eDomain)
		{
			if (num == 1)
				bindDSBuffer(slot, handles[0]);
			else
				bindDSBuffers(slot, handles, num);
		}
		if (shader & (uint32_t)Shader::ShaderType::eGeometry)
		{
			if (num == 1)
				bindGSBuffer(slot, handles[0]);
			else
				bindGSBuffers(slot, handles, num);
		}
		if (shader & (uint32_t)Shader::ShaderType::ePixel)
		{
			if (num == 1)
				bindPSBuffer(slot, handles[0]);
			else
				bindPSBuffers(slot, handles, num);
		}
		if (shader & (uint32_t)Shader::ShaderType::eCompute)
		{
			if (num == 1)
				bindCSBuffer(slot, handles[0]);
			else
				bindCSBuffers(slot, handles, num);
		}
	}

	template <Shader::ShaderType shader>
	inline void bindBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		if constexpr (shader == Shader::ShaderType::eVertex)
			bindVSBuffers(startSlot, handles, numBuffers);
		else if constexpr (shader == Shader::ShaderType::eHull)
			bindHSBuffers(startSlot, handles, numBuffers);
		else if constexpr (shader == Shader::ShaderType::eDomain)
			bindDSBuffers(startSlot, handles, numBuffers);
		else if constexpr (shader == Shader::ShaderType::eGeometry)
			bindGSBuffers(startSlot, handles, numBuffers);
		else if constexpr (shader == Shader::ShaderType::ePixel)
			bindPSBuffers(startSlot, handles, numBuffers);
		else if constexpr (shader == Shader::ShaderType::eCompute)
			bindCSBuffers(startSlot, handles, numBuffers);
		else
			static_assert(false);
	}

	inline void bindVSBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if (!isBufferBound<Shader::ShaderType::eVertex>(slot, handle))
		{
			m_d3d11Context->VSSetConstantBuffers(slot, 1, m_buffers[handle].buffer.GetAddressOf());
			m_boundVSBuffers[slot] = handle;
		}
	}

	inline void bindVSBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].buffer.Get();
			m_boundVSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->VSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

	inline void bindHSBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if (!isBufferBound<Shader::ShaderType::eHull>(slot, handle))
		{
			m_d3d11Context->HSSetConstantBuffers(slot, 1, m_buffers[handle].buffer.GetAddressOf());
			m_boundHSBuffers[slot] = handle;
		}
	}

	inline void bindHSBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].buffer.Get();
			m_boundHSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->HSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

	inline void bindDSBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if (!isBufferBound<Shader::ShaderType::eDomain>(slot, handle))
		{
			m_d3d11Context->DSSetConstantBuffers(slot, 1, m_buffers[handle].buffer.GetAddressOf());
			m_boundDSBuffers[slot] = handle;
		}
	}

	inline void bindDSBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].buffer.Get();
			m_boundDSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->DSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

	inline void bindGSBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if (!isBufferBound<Shader::ShaderType::eGeometry>(slot, handle))
		{
			m_d3d11Context->GSSetConstantBuffers(slot, 1, m_buffers[handle].buffer.GetAddressOf());
			m_boundGSBuffers[slot] = handle;
		}
	}

	inline void bindGSBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].buffer.Get();
			m_boundGSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->GSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

	inline void bindPSBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if (!isBufferBound<Shader::ShaderType::ePixel>(slot, handle))
		{
			m_d3d11Context->PSSetConstantBuffers(slot, 1, m_buffers[handle].buffer.GetAddressOf());
			m_boundPSBuffers[slot] = handle;
		}
	}

	inline void bindPSBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].buffer.Get();
			m_boundPSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->PSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

	inline void bindCSBuffer(uint32_t slot, ConstantBufferHandle handle)
	{
		if (!isBufferBound<Shader::ShaderType::eCompute>(slot, handle))
		{
			m_d3d11Context->CSSetConstantBuffers(slot, 1, m_buffers[handle].buffer.GetAddressOf());
			m_boundCSBuffers[slot] = handle;
		}
	}

	inline void bindCSBuffers(uint32_t startSlot, ConstantBufferHandle* handles, uint16_t numBuffers)
	{
		std::vector<ID3D11Buffer*> buffers(numBuffers);
		for (uint32_t i = 0; i < numBuffers; ++i)
		{
			const uint32_t& handle = handles[i];
			buffers[i] = m_buffers[handle].buffer.Get();
			m_boundCSBuffers[startSlot + i] = handle;
		}
		m_d3d11Context->CSSetConstantBuffers(startSlot, numBuffers, buffers.data());
	}

	inline void bindMaterial(uint32_t shader, ConstantBufferHandle handle)
	{
		bindBuffers(shader, MATERIAL_SLOT, &handle, 1);
	}

private:
	std::array<uint32_t, 16>						m_boundVSBuffers;
	std::array<uint32_t, 16>						m_boundHSBuffers;
	std::array<uint32_t, 16>						m_boundDSBuffers;
	std::array<uint32_t, 16>						m_boundGSBuffers;
	std::array<uint32_t, 16>						m_boundPSBuffers;
	std::array<uint32_t, 16>						m_boundCSBuffers;

	std::vector<ConstantBuffer>						m_buffers; // slot 0 is empty
};