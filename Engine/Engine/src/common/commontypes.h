#pragma once


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

}