#ifndef __BI_MAP_HH__
#define __BI_MAP_HH__
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
	typedef typename std::multimap<typename V, typename K, typename P>::iterator by_value_iterator;

	const V& value(const K& key) {
		return key_to_value_[key];
	}

    std::pair<by_value_iterator, by_value_iterator> keys(const V& value) {
        return value_to_key_.equal_range(value);
    }

    void set(const K& key, const V& value) {
        by_key_iterator it = key_to_value_.find(key);
        if (key_to_value_.end() != it) {
            std::pair<by_value_iterator, by_value_iterator> it_pair = value_to_key_.equal_range(key_to_value_[key]);
            while (it_pair.first != it_pair.second)
                if (it_pair.first->first == it->second) {
                    value_to_key_.erase(it_pair.first);
                    break;
                } else ++it_pair.first;
        }
        key_to_value_[key] = value;
        value_to_key_.insert(std::make_pair(value, key));
    }
};

#endif //__BI_MAP_HH__
