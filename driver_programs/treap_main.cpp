#include <iostream>
#include "treap.hpp"
#include "fftest.hpp"
#include <stdlib.h>

using namespace ff;

int main() {
	ff::treap<int, int>::test_rotate_left();
	ff::treap<int, int>::test_rotate_right();

	typedef ff::treap<int, int, std::less<int>, std::greater<int> > treap_t;
	treap_t t;

	for (int i = 0; i != 500; ++i) {
		t.insert_r(i, rand());
	}
	// erase is broken, need to be fixed
	test::say("Test delete structure:");
	for (int i = 0; i != 1000; ++i) {
		int k = rand()%1511;
		t.erase_r(k);
		test::verify(t.test_sanity(), "Treap structure is broken!");
	}

	test::say("Test structure:");
	test::verify(t.test_sanity(), "Treap structure is broken!");
	t.print("D:\\t.txt");
	test::tick();
	return 0;
}