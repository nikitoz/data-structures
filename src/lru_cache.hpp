#ifndef __FF_LRU_CACHE__
#define __FF_LRU_CACHE__
#include <map>
#include <list>

namespace ff {
template
<
	  typename TKey       // key type
	, typename TData      // data types to store
	, size_t   CAPACITY   // capacity
>
class lru_cache {
private:
	size_t                                    currently_stored_;
	std::list<TKey>                           age_;
	std::map<TKey, TData>                     cache_;
	std::map<TKey
		, typename std::list<TKey>::iterator> help_;

public:
	lru_cache() 
		: currently_stored_(0)
	{ }

	void put(const TKey& key, const TData& value) {
		if (currently_stored_ >= CAPACITY) {
			cache_.erase(age_.front());
			help_ .erase(age_.front());
			age_.pop_front();
			--currently_stored_;
		}
		++currently_stored_;
		age_.push_back(key);
		std::list<TKey>::iterator it = age_.end();
		std::advance(it, -1);
		help_ [key] = it;
	    cache_[key] = value;
	}

	const TData* get(const TKey& key) {
		if (cache_.find(key) == cache_.end())
			return 0;
		age_.erase(help_[key]);
		age_.push_back(key);
		std::list<TKey>::iterator it = age_.end();
		std::advance(it, -1);
		help_[key] = it;
		return &cache_[key];
	}
};
}      // ff
#endif // __FF_LRU_CACHE__