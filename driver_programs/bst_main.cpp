#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <set>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <memory.h>
#include <limits>
#include <string>
#include <sstream>
#include <queue>
#include <stdlib.h>
#include <list>
#include <stack>
#include "../src/bst.hpp"

int main() {
	typedef ff::bst<int, std::less<int> > bst_t;
	bst_t btree;
	int sorted[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	btree.from_sorted(sorted, sizeof(sorted)/sizeof(int));

	btree.remove(4);
	btree.remove(1);
	btree.remove(7);
	btree.remove(5);

	btree.print_postorder_r();
	btree.print_postorder();

	btree.print_inorder_r();
	btree.print_inorder();

	btree.print_preorder_r();
	btree.print_preorder();

	btree.remove(4);
	btree.remove(1);
	btree.remove(7);
	btree.remove(5);

	btree.print_inorder_r();
	btree.print_inorder();

	btree.print_preorder_r();
	btree.print_preorder();

	// 	printf("will remove 2 : ");
	// 	btree.remove(2);
	// 	btree.print_inorder();
	// 	printf("will remove 3 : ");
	// 	btree.remove(3);
	// 	btree.print_inorder();
	// 	printf("will remove 6 : ");
	// 	btree.remove(6);
	// 	btree.print_inorder();


	// 	bst_t::node_t* h = btree.dll();
	// 
	// 	while (h) {
	// 		printf("%d ", h->data_);
	// 		h = h->right_;
	// 	}
	// 	printf("\n");
	// 
	// 	printf("will remove 4 : ");
	// 	btree.erase(4);
	// 	btree.print_inorder();
	// 	printf("will remove 1 : ");
	// 	btree.erase(1);
	// 	btree.print_inorder();
	// 	printf("will remove 7 : ");
	// 	btree.erase(7);
	// 	btree.print_inorder();
	// 	printf("will remove 5 : ");
	// 	btree.erase(5);
	// 	btree.print_inorder();
	// 	printf("will remove 2 : ");
	// 	btree.erase(2);
	// 	btree.print_inorder();
	// 	printf("will remove 3 : ");
	// 	btree.erase(3);
	// 	btree.print_inorder();
	// 	printf("will remove 6 : ");
	// 	btree.erase(6);
	// 	btree.print_inorder();
	return 0;
}
