#include <algorithm>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <set>
#include <cassert>
#include <ctime>
#include <functional>
#include <cmath>
#include <memory.h>
#include "../src/trie.hpp"
#include <iostream>

#define eps 1e-9

#define forn(i, n) for(int i = 0; i < (int)(n); i++)
#define sz(v)((v).size())
#define mp(x, y) make_pair(x, y)
#define ll_t long long

void  ff_Verify(bool b, char* ch = 0) {
	std::cout << (b ? "OK" : "FAIL ") << ( b ? "" : ch) << std::endl;
}

using namespace std;

void test_trie() {
	ff::trie<> t;
	std::vector<std::pair<std::string, int> > v;
	v.push_back(std::make_pair("abba",  1));
	v.push_back(std::make_pair("abbaz", 2));
	v.push_back(std::make_pair("aaba",  3));
	v.push_back(std::make_pair("abb",   4));
	v.push_back(std::make_pair("abbat",  5));

	for (int i = 0; i != (int)v.size(); ++i) {
		t.insert(v[i].first.c_str(), v[i].second);
		ff_Verify (t.test_count(), "test count failed");
	}

	for (int i = 0; i != (int)v.size(); ++i)
		ff_Verify (v[i].second == t.get(v[i].first.c_str()), "was found incorrectly");
	
	for (int i = (int)v.size()-1; i >= 0; --i) {
		t.remove(v[0].first.c_str());
		ff_Verify (t.test_count(), "test count failed");
	}
}

int main() {
	test_trie();
	return 0;
}
