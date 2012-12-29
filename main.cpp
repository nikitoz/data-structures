#include "AVLTree.h"
#include "AVLTree_test.h"
#include <iostream>
#include <istream>
#include <map>
#include<ctime>
#include "HRTimer.h"

#define TEST_ELEMENTS_NUM 40000

typedef ff_amap< int, int, std::less<int> > MapTest;
typedef std::map<int, int> StdMap;

inline int random() {
	return rand();
}

bool testLookup(MapTest &mapy, StdMap& stdMap) {
	bool bRetVal = true;
	HRTimer tim;
	tim.StartTimer();
	for(int i = 0; i < TEST_ELEMENTS_NUM; ++i) {
		int k = random();
		mapy[k] = k;
		stdMap[k] = k;
	}
	std::cout << tim.StopTimer() << std::endl;

	for(StdMap::iterator it = stdMap.begin(); it != stdMap.end(); ++it) {
		if (mapy[it->first] != it->second) {
			std::cout << "!!! Lookup failed !!!" << std::endl;
			bRetVal = false;
		}
	}

	if (!mapy.checkChildren()) {
		std::cout << "!!! Children checking failed !!!" << std::endl;
		bRetVal = false;
	}

	if (!mapy.checkBalancing(TEST_ELEMENTS_NUM)) {
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

	if (!mapy.checkChildren()) {
		std::cout << "!!! Children checking failed !!!" << std::endl;
		bRetVal = false;
	}

	if (!mapy.checkBalancing(TEST_ELEMENTS_NUM)) {
		printf("!!! Balancing fail !!!");
		bRetVal = false;
	}

	for(StdMap::iterator it = stdMap.begin(); it != stdMap.end(); ++it) {
		if (mapy[it->first] != it->second) {
			std::cout << "!!! Lookup failed !!!" << std::endl;
			bRetVal = false;
		}
	}
	int cnt = mapy.count();

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
		if (testDelete(avlTree, stdMap)) {
			std::cout << "Delete success" << std::endl;
		}
		FF_AVL_GraphCreator::createDotFile<ff_amap< int, int, std::less<int> >
			, anode<int, int> >(&avlTree, "graphAfter");
		printf("OK!");
	} else {
		printf("Test Failed\n");
		
	}
	std::cin.get();
	return 0;
}