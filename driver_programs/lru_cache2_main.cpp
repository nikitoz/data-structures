#include "../src/lru_cache2.hpp"
#include <iostream>
#include <vector>
#include <string>

void  ffverify(bool b, char* ch = 0) {
	std::cout << (b ? "OK" : "FAIL ") << ( b ? "" : ch) << std::endl;
}

int main() {
	ff::treap<int, int> t;

	/*t.insert_r(1, 2);
	t.insert_r(3, 1);
	t.insert_r(2, 0);

	t.erase_r(2);

	t.insert_r(4, 3);*/



	ff::lru_cache2<int, std::string, 3> cache;
	cache.put(1, "a");
	cache.put(3, "c");
	cache.put(2, "b");
	cache.put(4, "d");
	cache.get(2);
	cache.get(3);
	cache.put(5, "e");
	ffverify(0==cache.get(4), "Four is not removed");
	ffverify(0!=cache.get(3), "3");
	ffverify(0!=cache.get(2), "2");
	return 0;
}
