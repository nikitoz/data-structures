/*
	This is simple 2d vector container, which uses RLE for saving data.
	It takes std::vector on input, codes it with RLE, then stores.
	get() returns original decoded data.
*/
#ifndef __FF_RLE_CACHE__
#define __FF_RLE_CACHE__
#include <vector>
#include <utility>

namespace ff {
template 
<
	typename T // Type of the values should have operator==
>
class rle_cache
{
	typedef std::vector<T>             in_t;
	typedef std::pair<size_t, T>       rle_tuple_t;
	typedef std::vector< rle_tuple_t > out_t;

public:
	rle_cache()
	{ }

	void append(const in_t& v)
	{
		if (v.empty())
			return;

		in_t::const_iterator previous = v.begin();
		_d.push_back(out_t());
		out_t& result = _d.at(_d.size() - 1);

		size_t count = 1;
		for (in_t::const_iterator it = v.begin() + 1; it != v.end(); ++it)
		{
			if (*it != *previous) 
			{
				result.push_back(std::make_pair<size_t, T>(count, *previous));
				count = 1;
			}
			else
				count++;
			previous = it;
		}
		result.push_back(std::make_pair<size_t, T>(count, *previous));
		_sizes.push_back(v.size());
	}

	void get(size_t index, in_t& out)
	{
		if (index >= _d.size())
			return out.clear(); // yep
		out.resize(_sizes[index]);
		out_t& coded = _d.at(index);
		in_t::iterator mediator = out.begin();

		for (out_t::const_iterator it = coded.begin(); it != coded.end(); ++it)
		{
			std::fill_n (mediator, it->first, it->second);
			std::advance(mediator, it->first);
		}
	}

private:
	std::vector< out_t >   _d;
	std::vector< size_t >  _sizes;
};// rle_cache
} // ff
#endif //__FF_RLE_CACHE__