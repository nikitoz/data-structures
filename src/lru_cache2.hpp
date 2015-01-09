#ifndef __FF_LRU_CACHE__
#define __FF_LRU_CACHE__
#include "treap.hpp"

/*
 * This is an experimental implementation of the LRU cache using treap
 * as an internal buffer.Autoincremental ids are used as a heap key.
 * Every time an object is retrieved or inserted to a cache,
 * maximum id is set to it. Least recently used object is the one with minimum id,
 * will be the root of a treap.
*/
namespace ff {
template
<
	  typename TKey
	, typename TData
>
class lru_cache2
{
	typedef TKey                        key_t;
	typedef TData                       data_t;
	typedef unsigned long long          bigint_t;
	typedef std::pair<data_t, bigint_t> heap_key_t;

	struct heap_predicate {
		bool operator()(const heap_key_t& k1, const heap_key_t& k2) const {
			return k1.second < k2.second;
		}
	};

	typedef std::less<key_t>           key_predicate_t;
	typedef heap_predicate             heap_predicate_t;
	typedef treap<
			  key_t
			, heap_key_t
			, key_predicate_t
			, heap_predicate_t>        treap_t;
	typedef typename treap_t::iterator iterator;

public:
	lru_cache2(size_t capacity)
		: capacity_(capacity)
		, auto_increment_field_(0)
	{ }

	const data_t* put(const key_t& key, const data_t& value) {
		if (capacity_ <= cache_.count())
			cache_.erase_r(cache_.top());
		iterator it = cache_.insert_r(key, std::make_pair(value, auto_increment_field_++));
		return &it->second.first;
	}

	const data_t* get(const key_t& key) {
		data_t* retval = 0;
		iterator it = cache_.find_r(key);
		if (it != cache_.end()) {
			retval = &it->second.first;
			it->second.second = auto_increment_field_++;
			cache_.heapify_r(it);
		}
		return retval;
	}

private:
	treap_t  cache_;
	size_t   capacity_;
	bigint_t auto_increment_field_;
};     // lru_cache2
}      // ff
#endif // __FF_LRU_CACHE__
