#pragma once

#include <memory>
#include <mutex>

template <class type>
class Singletone
{
public:
	Singletone() {};
	~Singletone() {};

public:
	static type* Get();
	virtual void Create() {};

private:
	static std::unique_ptr<type> m_singletoneInstance;
	static std::once_flag m_singletoneFlags;
};

template <class type>
std::unique_ptr<type> Singletone<type>::m_singletoneInstance = nullptr;
template <class type>
std::once_flag Singletone<type>::m_singletoneFlags;

template<class type>
inline type* Singletone<type>::Get()
{
	std::call_once(m_singletoneFlags, [&]
	{
		m_singletoneInstance = std::make_unique<type>(); 
		m_singletoneInstance->Create();
	});
	return m_singletoneInstance.get();
}
