#include "bheap.hpp"
#include <iostream>
//=========================================================================
// Test routines
//=========================================================================

using namespace ff;
namespace
{ typedef bheap<int> IntHeap; }

void printHeap(const IntHeap& bh) {
	const int* out = bh.getArray();
	size_t n = bh.getSize();
	for (size_t i = 0; i < n; ++i)
		std::cout << out[i] << std::endl;

	std::cout << std::endl;
	bh.checkHeapProperty();
}

bool testHeap() {
	IntHeap bhh;

	int k = 0;
	for(int i = 0; i < 100; ++i) {
		int r = rand();
		bhh.insert(r);
		bhh.checkHeapProperty();
		if (k == 5) {
			k = 0;
			bhh.pop();
			if (!bhh.checkHeapProperty())
				return false;
		}
		k++;
	}

	for (size_t i = bhh.getSize(); i != 0; --i) {
		bhh.pop();
		if (!bhh.checkHeapProperty())
			return false;
	}
	return true;
}
/*
int main() {
	if (!testHeap())
		std::cout << "!!! Heap test failed !!!" << std::endl;
	else
		std::cout << "Heap test success" << std::endl;
	std::cin.get();
	return 0;
}*/