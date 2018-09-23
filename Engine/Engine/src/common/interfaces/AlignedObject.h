#pragma once


#include "../../common/common.h"


__declspec(align(16)) class IAlignedObject
{
public:
	inline void* operator new( size_t size )
	{
		return _aligned_malloc(size,16);
	};
	inline void operator delete( void* block )
	{
		_aligned_free(block);
	};
};