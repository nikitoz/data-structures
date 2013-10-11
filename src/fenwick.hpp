#ifndef __FF__FENWICK__
#define __FF__FENWICK__

#include <vector>
#include "fftest.hpp"

namespace ff {
template 
<
	  typename T
	, typename TFun
	, typename TFunRev
>
class fenwick {
private:
	std::vector<T> d_;
	TFun           fun_;
	TFunRev        fun_rev_;

public:
	typedef int size_type; // use long long on x64

	fenwick(size_type size)
		: d_(size, T())
	{ }

	void insert(size_type i, const T& t) {
		FFASSERT( i >= 0 && i < (size_type)d_.size() );
		d_[i] = fun_(d_[i], t);
		if (i == 0)
			return;
		size_type idx = child_index(i);
		while (idx < (size_type)d_.size()) {
			d_[idx] = fun_(d_[idx], t);
			idx = child_index(idx);
		}
	}

	T accumulate(size_type i) const {
		FFASSERT( i >= 0 && i < (size_type)d_.size() );
		T ret = d_[0];
		while (i > 0) {
			ret = fun_(ret, d_[i]);
			i = parent_index(i);
		}
		return ret;
	}

	T accumulate(size_type begin, size_type end) const {
		FFASSERT( begin >= 0 && begin < (size_type)d_.size() && end >= 0 && end < (size_type)d_.size() && begin < end );
		return accumulate(end + 1) - accumulate(begin - 1);
	}

	T value_bandit(size_type index) const {
		FFASSERT( index >= 0 && index < (size_type)d_.size() );
		if (0 == index)
			return d_[index];
		return accumulate(index) - accumulate(index-1);
	}

	T value(size_type index) const {
		FFASSERT( index >= 0 && index < (size_type)d_.size() );
		if (0 == index)
			return d_[0];
		T prob = d_[index];
		size_type index_parent = parent_index(index);
		--index;
		while (index_parent != index) {
			prob = fun_rev_(prob, d_[index]);
			index = parent_index(index);
		}
		return prob;
	}

private:
	inline size_type parent_index(size_type index) const {
		return index&(index - 1);
	}

	inline size_type child_index(size_type index) const {
		return index + (index&(-index));
	}

	fenwick()
	{ }
};     //   fenwick
};     //   ff
#endif // __FF__FENWICK__