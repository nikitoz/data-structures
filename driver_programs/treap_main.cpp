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

	for (int i = 0; i != 30; ++i) {
		t.insert_r(i, rand());
	}
	// erase is broken, need to be fixed
	t.erase_r(0);
	t.erase_r(10);
	t.erase_r(12);
	t.erase_r(13);
	t.erase_r(14);
	t.erase_r(25);
	test::say("Test structure:");
	test::verify(t.test_sanity(), "Treap structure is broken!");
	t.print("D:\\t.txt");
	test::tick();
	return 0;
}