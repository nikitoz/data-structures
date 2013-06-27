// rle_cache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rle_cache.hpp"
#include <iostream>

bool test_cache()
{
	ff::rle_cache<int> cache;
	typedef std::vector<int> vec_t;

	vec_t v;

	v.push_back(1);
	v.push_back(2);
	v.push_back(2);
	v.push_back(3);
	v.push_back(3);
	v.push_back(3);
	v.push_back(0);
	v.push_back(0);

	cache.append(v);

	vec_t vv;
	cache.get(0, vv);

	if (vv!=v)
		std::cout << "FAIL";

	v.clear();
	v.push_back(0);
	cache.append(v);
	cache.get(1, vv);

	if (vv!=v)
		std::cout << "FAIL";

	v.clear();
	v.push_back(0);
	v.push_back(0);
	cache.append(v);
	cache.get(2, vv);
	if (vv!=v)
		std::cout << "FAIL";

	v.clear();
	v.push_back(1);
	v.push_back(2);
	cache.append(v);
	cache.get(3, vv);
	if (vv!=v)
		std::cout << "FAIL";

	v.clear();
	cache.append(v);
	cache.get(4, vv);
	if (vv!=v)
		std::cout << "FAIL";

	return true;
}

int main()
{
	test_cache();
	return 0;
}

