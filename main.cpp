#include "AVLTree.h"
#include "AVLTree_test.h"
#include <iostream>
#include <istream>

#define TEST_ELEMENTS_NUM 10

int main(){
	FF_AVLTree_test test;
	if (test.testLookUp() && test.testRotate() && test.testTraverse()){
		ff_amap< int, int, less<int> > avlTree;
		int a = 0;

			for(int i = 0; i < TEST_ELEMENTS_NUM; ++i){
				avlTree.insertNode(i,i);
			}
			FF_AVL_GraphCreator::createDotFile<ff_amap< int, int, less<int> >
				,anode<int, int> >(&avlTree, "graphBefore");

			for (int i = 0; i < TEST_ELEMENTS_NUM; ++i ){
				int val;
				try{
					val = avlTree.findNode(i);
				} catch(std::exception& e){printf("%s", e.what());}
			}

			for (int i = 0; i < 10; ++i ){
				int k = rand()%10; // I don't care that this doesn't produce uniform distribution
				avlTree.deleteNode(k);
				char n[256] = {0};
				char ch[10] = {0};
				strcpy(n, "graph");
				strcat(n, itoa(i, ch, 10));
				FF_AVL_GraphCreator::createDotFile<ff_amap< int, int, less<int> >
					,anode<int, int> >(&avlTree, n);
			}
			FF_AVL_GraphCreator::createDotFile<ff_amap< int, int, less<int> >
				,anode<int, int> >(&avlTree, "graphAfter");

		printf("OK!");
	} else {
		printf("Test Failed\n");
		std::cin.get();
	}
	return 0;
}