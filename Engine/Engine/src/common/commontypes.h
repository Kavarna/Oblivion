#pragma once

#include "boost/operators.hpp"

namespace CommonTypes
{

	template <class indexType>
	struct range_t
	{
		indexType begin;
		indexType end;

		range_t() = default;
		range_t(indexType begin, indexType end) :
			begin(begin), end(end) {};
	};

	using Range = range_t<uint32_t>;

	struct RayHitInfo : public boost::less_than_comparable<RayHitInfo>
	{
		float dist;
		int32_t instanceID;
		bool operator < (const RayHitInfo & rhs) const
		{
			return dist < rhs.dist;
		}
	};

}