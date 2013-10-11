#include "../src/fenwick.hpp"
#include <vector>
#include <iostream>
#include "../src/fftest.hpp"
#include <numeric>

struct sum {
	int operator()(const int& a1, const int& a2) const {
		return a1+a2;
	}
};

struct minus {
	int operator()(const int& a1, const int& a2) const {
		return a1-a2;
	}
};
using namespace ff;
int main() {
	int input[] = {1, 0, 2, 1, 1, 3, 0, 4, 2, 3, 4, 5, 7, 0, 3, 12, 5, 2, 3 };
	int cnt = sizeof(input)/sizeof(int);
	ff::fenwick<int, sum, minus> f(cnt);
	for (int i = 0; i != cnt; ++i)
		f.insert(i, input[i]);

	ff::test::say("testing accumulate");
	for (int i = 0; i != cnt; ++i)
		test::verify(f.accumulate(i) == std::accumulate(input,input+i+1, 0), "not equals");

	test::say("testing single value");
	for (int i = 0; i != cnt; ++i)
		test::verify(f.value(i) == input[i], "single value not equals");
	test::tick();
	return 0;
}
