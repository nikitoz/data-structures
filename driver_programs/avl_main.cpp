
#include <iostream>
#include <map>
#include <ctime>
#include "HRTimer.h"
#include "../_tests/avltree_test.h"
#include "../src/avl.hpp"

#define TEST_ELEMENTS_NUM 40000

typedef ff::amap< int, int, std::less<int> > MapTest;
typedef std::map<int, int> StdMap;

inline int random() {
	return rand();
}

bool testIterator(MapTest &mapy, StdMap& stdMap) {

	MapTest::iterator it1 = mapy.begin();
	StdMap::iterator  it2 = stdMap.begin();

	while(it2 != stdMap.end()) {
		if (it1->val != it2->second) {
			std::cout << "!!! Iterator test failed !!!" << std::endl;
			return false;
		}
		++it1;
		++it2;
	}
	
	return true;
}

bool testLookup(MapTest &mapy, StdMap& stdMap) {
	bool bRetVal = true;
	HRTimer tim;
	tim.StartTimer();
	for(int i = 0; i < TEST_ELEMENTS_NUM; ++i) {
		int k = random();
		mapy.insert(k, k);
		stdMap[k] = k;
	}
	std::cout << tim.StopTimer() << std::endl;

	for(StdMap::iterator it = stdMap.begin(); it != stdMap.end(); ++it) {
		const int* pE = mapy.nodePtr(it->first);
		if (!pE || *pE != it->second) {
			std::cout << "!!! Lookup failed !!!" << std::endl;
			bRetVal = false;
		}
	}

	if (!mapy.check_parents()) {
		std::cout << "!!! Parent checking failed !!!" << std::endl;
		bRetVal = false;
	}

	if (!mapy.check_children()) {
		std::cout << "!!! Children checking failed !!!" << std::endl;
		bRetVal = false;
	}

	if (!mapy.check_balancing(TEST_ELEMENTS_NUM)) {
		printf("!!! Balancing fail !!!");
		bRetVal = false;
	}
	return bRetVal;
}

bool testDelete(MapTest &mapy, StdMap& stdMap) {
	bool bRetVal = true;
	int del = 0;
	HRTimer tim;
	tim.StartTimer();
	for(int i = 0; i < TEST_ELEMENTS_NUM; ++i) {
		int k = random();
		if(	mapy.erase(k) ) {
			del ++;
			stdMap.erase(stdMap.find(k));
		}
	}
	std::cout << "Deletion time : " << tim.StopTimer() << std::endl;

	if (!mapy.check_parents()) {
		std::cout << "!!! Parent checking failed !!!" << std::endl;
		bRetVal = false;
	}

	if (!mapy.check_children()) {
		std::cout << "!!! Children checking failed !!!" << std::endl;
		bRetVal = false;
	}

	if (!mapy.check_balancing(TEST_ELEMENTS_NUM)) {
		printf("!!! Balancing fail !!!");
		bRetVal = false;
	}

	for(StdMap::iterator it = stdMap.begin(); it != stdMap.end(); ++it) {
		if (mapy[it->first] != it->second) {
			std::cout << "!!! Lookup failed !!!" << std::endl;
			bRetVal = false;
		}
	}
	int cnt = mapy.dcount();

	if (
		(cnt != stdMap.size())
		) {
		std::cout << "!!! Deletion error !!!" << std::endl;
		bRetVal = false;
	} 

	if (bRetVal) {
		std::cout << "Actually deleted " << del << std::endl;
	}
	return bRetVal;
}

int main() {
	FF_AVLTree_test test;
	if (test.testLookUp() && test.testRotate() && test.testTraverse()) {
		int a = 0;
		int actualydeleted = 0;

		StdMap  stdMap;
		MapTest avlTree;
		
		if (testLookup(avlTree, stdMap)) {
			std::cout << "Lookup success" << std::endl;
		}

		if (testIterator(avlTree, stdMap)) {
			std::cout << "Iterator test passed" << std::endl;
		}

		if (testDelete(avlTree, stdMap)) {
			std::cout << "Delete success" << std::endl;
		}
		FF_AVL_GraphCreator::createDotFile<ff::amap< int, int, std::less<int> >
			, ff::amap< int, int, std::less<int> >::node_t >(&avlTree, "graphAfter");
		printf("OK!");
	} else {
		printf("Test Failed\n");
		
	}
	std::cin.get();
	return 0;
}
