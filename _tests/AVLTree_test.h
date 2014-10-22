#ifndef __FF_AVL_TEST__
#define __FF_AVL_TEST__
#include <stack>
#include "../src/avl.hpp"
//=========================================================================
// Unit tests for avl tree implementation
//=========================================================================
using namespace ff;
class FF_AVLTree_test {
typedef ff::amap<int, int>::node_t anode;
public:
	ff::amap<int, int> avl;
	FF_AVLTree_test(){}
	~FF_AVLTree_test(){}
//=========================================================================
// tests rotations
//=========================================================================
	bool testRotate(){
		bool retVal = false;
		anode* l = new anode(10,10);l->childCount = 1;
		anode* c = new anode(11,11);l->childCount = 201;
		anode* r = new anode(12,12);l->childCount = 3;
		anode* m = new anode(13,13);l->childCount = 450;
		anode* n = new anode(14,14);l->childCount = 1335;

		do {
			//test little left
			anode a = anode(0,0);
			anode b = anode(1,1);

			a.left = (anode*)l;
			b.left = (anode*)c;
			b.right = (anode*)r;
			a.right = &b;

			anode* aa = avl.z_single_left_rotate(&a);

			if (!(a.left == l && a.right == c && a.first == 0 && a.second == 0 
				&& b.left == &a && b.right == r && b.first == 1 && b.second == 1) && (&b == aa))
				return false;

		} while(false);

		do {
			//test little right
			anode a = anode(0,0);
			anode b = anode(1,1);

			a.left =  &b;
			b.left = (anode*)l;
			b.right = (anode*)c;
			a.right = (anode*)r;
			b.update();
			a.update();
			anode* aa = &a;
			avl.z_single_left_rotate(aa);

			if (!(a.left == c && a.right == r && a.first == 0 && a.second == 0 
				&& b.left == l && b.right == &a && b.first == 1 && b.second == 1
				&& a.childCount == (r->childCount + c->childCount + 2) 
				&& b.childCount == (a.childCount + l->childCount + 2) 
				&& (a.balance  == (c->childCount - r->childCount))
				&& (b.balance == (l->childCount - a.childCount))))
				return false;

		} while (false);

		do {
			anode* a = new anode(0,0);
			anode* b = new anode(1,1);

			a->left = (anode*)l;
			a->right = b;

			b->left = (anode*)c;
			b->right = (anode*)r;
			c->left = m;
			c->right = n;

			anode* rr = NULL;
			rr = avl.z_double_left_rotate(a);

			if (!((rr->left == a) && (rr->right == b) && (rr->left->left == l) && (rr->left->right == m) 
				&& (rr->right->left == n) && (rr->right->right == r)
				&& (rr == c))) {
				return false;
			}

		} while(false);

		do {
			anode* a = new anode(0,0);
			anode* b = new anode(1,1);

			a->left = b;
			a->right = r;

			b->left = l;
			b->right = c;
			c->left = m;
			c->right = n;

			anode* rr = NULL;
			rr = avl.z_double_right_rotate(a);

			if (!((rr->left == b) && (rr->right == a) && (rr->left->left == l) && (rr->left->right == m) 
				&& (rr->right->left == n) && (rr->right->right == r)
				&& (rr == c))) {
					return false;
			}

		} while(false);

		return true;
	}
//=========================================================================
// findLeftMostChild, findRightMostChild
//=========================================================================
	/*bool testTraverse(){
		anode* a = new anode(10,10);
		anode* b = new anode(11,11);
		anode* c = new anode(12,12);
		anode* d = new anode(13,13);
		anode* e = new anode(14,14);

		anode* eee = new anode(141,141);

		anode* root=NULL;
		anode** a_addr = &a;
		ff::amap<int, int>::panode_t* paa[4000];int N = 0;
		anode<int, int>** l_addr = avl.findLeftMostChild(a, paa, N) ;

		if ((avl.findLeftMostChild(a, paa, N) != &a) || (avl.findRightMostChild(a, paa, N) != &a)) {
			return false;
		}

		a->left = b;
		a->right = c;
		if (avl.findLeftMostChild(a, paa, N) != &(a->left)) {
			return false;
		}
		if (avl.findRightMostChild(a, paa, N) != &(a->right)) {
			return false;
		}

		b->left = d;
		b->right = e;
		if (avl.findLeftMostChild(a, paa, N) != &(b->left)) {
			return false;
		}
		b->left = NULL;
		b->right = NULL;

		c->left = d;
		c->right = e;
		if (avl.findRightMostChild(a, paa, N) != &(c->right)) {
			return false;
		}
		c->left = NULL;

		e->right = d;
		d->right = eee;

		if (avl.findRightMostChild(a, paa, N) != &(d->right)) {
			return false;
		}

		return true;
	}

//=========================================================================
// findNodePtrPtr
//=========================================================================
	bool testLookUp(){
		anode<int, int>* a = new anode<int, int>(10,10);
		anode<int, int>* b = new anode<int, int>(9,9);
		anode<int, int>* c = new anode<int, int>(13,13);
		anode<int, int>* d = new anode<int, int>(11,11);
		anode<int, int>* e = new anode<int, int>(14,14);

		anode<int, int>* eee = new anode<int, int>(12,12);
		anode<int, int>* root=NULL;

		a->left = b;
		a->right = c;
		bool result = false;
		ff_amap<int, int>::panode_t* paa[4000];int N;
		anode<int, int>** pa = &a->left;
		if (pa != avl.findNodePtrPtr(&a,9, paa, N)) {
			return false;
		}

		c->left = eee;
		c->left->left = d;

		pa = &a->right;

		if (pa != avl.findNodePtrPtr(&a, 13, paa, N)) {
			return false;
		}

		c->right = e;
		pa = &c->right;
		if (pa != avl.findNodePtrPtr(&a, 14, paa, N)) {
			return false;
		}

		pa = &c->left;

		if (pa != avl.findNodePtrPtr(&a, 12, paa, N) ) {
			return false;
		}

		return true;
	}
*/
};
//=========================================================================
// Creation files with dot-language description of the trees
//=========================================================================
class FF_AVL_GraphCreator {
public:
	//=========================================================================
	// Call this function to create file, then use
	// "c:\Program Files (x86)\Graphviz 2.28\bin\dot.exe" -v -Tpng -ooutputFileName.png fileName
	// to create images with your AVL-tree (!!! Can be used only with integer keys !!!)
	//=========================================================================
	template < typename TTree, typename TRoot >
	static void createDotFile(const TTree * const tree, const char* fileName, const int key = -1) {
		FILE*f=fopen(fileName, "wt");
		if (f != NULL){
			char temp[256] = {0};
			int j = 0;
			for(int i = 0; i < strlen(fileName); ++i) if(fileName[i] != '.') temp[j++] = fileName[i];
			if (j == 0) temp[j] = 'G';
			fprintf(f, "digraph %s{\n", temp);
			if (key != -1)
				fprintf(f, "%d->%d;\n", key, key);
			TRoot* root = tree->root_;
			createRecord<TRoot>(root, f);
			fprintf(f, "}");
		}
		fclose(f);
	}
private:
	template <typename TRoot>
	static void createRecord(const TRoot* const root, FILE*file) {
		if (root != NULL && file != NULL){
			if (root->right != NULL) 
				fprintf(file,"%d->%d;\n", root->first, root->right->first);

			if (root->left != NULL)
				fprintf(file,"%d->%d;\n", root->first, root->left->first);

			if(root->right != NULL)
				createRecord<TRoot>(root->right, file);
			
			if (root->left != NULL)
				createRecord<TRoot>(root->left, file);
		}
	}
};

#endif