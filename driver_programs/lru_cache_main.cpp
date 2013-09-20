#include "lru_cache.hpp"
#include <iostream>

void  ffverify(bool b, char* ch = 0) {
	std::cout << (b ? "OK" : "FAIL ") << ( b ? "" : ch) << std::endl;
}

int main() {
	ff::lru_cache<int, int, 3> cache;
	cache.put(1, 1);
	cache.put(2, 2);
	cache.put(3, 3);
	cache.put(4, 4);
	cache.get(2);
	cache.get(3);
	cache.put(5, 5);
	ffverify(0==cache.get(4), "Four is not removed");
	ffverify(0!=cache.get(3), "3");
	ffverify(0!=cache.get(2), "2");
	return 0;
}