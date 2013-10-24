#ifndef __VALUE_ORDERED_MAP_HH__
#define __VALUE_ORDERED_MAP_HH__
#include <map>
#include <utility>
#include <functional>
template
<
	typename K, // key
	typename V, // value
	typename P = std::less<V>  // predicate
>
class value_ordered_map
{
private:
	std::map<K, V>         key_to_value_;
	std::multimap<V, K, P> value_to_key_;

public:
	typedef typename std::multimap<typename V, typename K, typename P>::iterator sorted_by_value_iterator;

	const V& value(const K& key) {
		return key_to_value_[key];
	}

	void set(const K& key, const V& value) {
		sorted_by_value_iterator it = value_to_key_.find(key_to_value_[key]);
		if (it != value_to_key_.end())
			value_to_key_.erase(it);
		key_to_value_[key] = value;
		value_to_key_.insert(std::make_pair(value, key));
	}

	std::pair<sorted_by_value_iterator, sorted_by_value_iterator>
	sorted_by_value_range() {
		return std::make_pair(value_to_key_.begin(), value_to_key_.end());
	}
};

#endif //__VALUE_ORDERED_MAP_HH__