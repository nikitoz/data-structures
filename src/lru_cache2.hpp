#ifndef __FF_LRU_CACHE__
#define __FF_LRU_CACHE__
#include <map>
#include <list>
#include "treap.hpp"

namespace ff {
template
<
	  typename TKey       // key type
	, typename TData      // data types to store
	, size_t   CAPACITY   // capacity
>
class lru_cache2 {
	typedef TKey                       key_t;
	typedef TData                      data_t;
	typedef unsigned long long         bigint_t;
	typedef std::pair<TData, bigint_t> heap_key_t;

public:
	lru_cache2()
		: auto_increment_field_(0)
	{ }

	void put(const key_t& key, const data_t& value) {
		if (CAPACITY <= cache_.count())
			cache_.erase_r(cache_.top());
		cache_.insert_r(key, std::make_pair(value, auto_increment_field_++));
	}

	const data_t* get(const key_t& key) {
		data_t* retval = 0;
		treap_t::iterator it = cache_.find_r(key);
		if (it != cache_.end()) {
			it->second.second = auto_increment_field_++;
			retval = &it->second.first;
			cache_.heapify_r(it);
		}
		return retval;
	}

private:
	struct heap_predicate {
		bool operator()(const heap_key_t& k1, const heap_key_t& k2) const {
			return k1.second > k2.second;
		}
	};

	typedef std::less<key_t> key_predicate_t;
	typedef heap_predicate   heap_predicate_t;
	typedef treap<key_t, heap_key_t, key_predicate_t, heap_predicate_t>  treap_t;

	treap_t  cache_;
	bigint_t auto_increment_field_;
};     // lru_cache2
}      // ff
#endif // __FF_LRU_CACHE__