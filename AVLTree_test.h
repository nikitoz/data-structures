#ifndef __FF_AVL_TEST__
#define __FF_AVL_TEST__
#include <stack>
#include "AVLTree.h"
//=========================================================================
// Unit tests for avl tree implementation
//=========================================================================
class FF_AVLTree_test{
public:
	ff_amap<int, int> avl;
	FF_AVLTree_test(){}
	~FF_AVLTree_test(){}
//=========================================================================
// tests rotations
//=========================================================================
	bool testRotate(){
		bool retVal = false;
		anode<int, int>* l = new anode<int, int>(10,10);l->childCount = 1;
		anode<int, int>* c = new anode<int, int>(11,11);l->childCount = 201;
		anode<int, int>* r = new anode<int, int>(12,12);l->childCount = 3;
		anode<int, int>* m = new anode<int, int>(13,13);l->childCount = 450;
		anode<int, int>* n = new anode<int, int>(14,14);l->childCount = 1335;

		do {
			//test little left
			anode<int, int> a = anode<int, int>(0,0);
			anode<int, int> b = anode<int, int>(1,1);

			a.left = (anode<int, int>*)l;
			b.left = (anode<int, int>*)c;
			b.right = (anode<int, int>*)r;
			a.right = &b;

			anode<int, int>** aa;
			anode<int, int>* aaa = &a;
			aa = &aaa;

			avl.littleLeftRotate(aa);

			if (!(a.left == l && a.right == c && a.key == 0 && a.val == 0 
				&& b.left == &a && b.right == r && b.key == 1 && b.val == 1
				&& (&b == *aa)))
				return false;

		} while(false);

		do {
			//test little right
			anode<int, int> a = anode<int, int>(0,0);
			anode<int, int> b = anode<int, int>(1,1);

			a.left =  &b;
			b.left = (anode<int, int>*)l;
			b.right = (anode<int, int>*)c;
			a.right = (anode<int, int>*)r;
			b.calcChildCount();b.calcBalance();
			a.calcChildCount();a.calcBalance();
			anode<int, int>* aa = &a;
			avl.littleRightRotate(&aa);

			if (!(a.left == c && a.right == r && a.key == 0 && a.val == 0 
				&& b.left == l && b.right == &a && b.key == 1 && b.val == 1
				&& (&b == aa) 
				&& a.childCount == (r->childCount + c->childCount + 2) 
				&& b.childCount == (a.childCount + l->childCount + 2) 
				&& (a.balance  == (c->childCount - r->childCount))
				&& (b.balance == (l->childCount - a.childCount))))
				return false;

		} while (false);

		do{
			anode<int, int>* a = new anode<int, int>(0,0);
			anode<int, int>* b = new anode<int, int>(1,1);

			a->left = (anode<int, int>*)l;
			a->right = b;

			b->left = (anode<int, int>*)c;
			b->right = (anode<int, int>*)r;
			c->left = m;
			c->right = n;

			anode<int, int>* rr = a;
			avl.bigLeftRotate(&rr);

			if (!(
				(rr->left == a) && (rr->right == b) && (rr->left->left == l) && (rr->left->right == m) && (rr->right->left == n) && (rr->right->right == r)
				&& (rr == c)
				)){
				return false;
			}

		} while(false);

		do{
			anode<int, int>* a = new anode<int, int>(0,0);
			anode<int, int>* b = new anode<int, int>(1,1);

			a->left = b;
			a->right = r;

			b->left = l;
			b->right = c;
			c->left = m;
			c->right = n;

			anode<int, int>* rr = a;
			avl.bigRightRotate(&rr);

			if (!(
				(rr->left == b) && (rr->right == a) && (rr->left->left == l) && (rr->left->right == m) && (rr->right->left == n) && (rr->right->right == r)
				&& (rr == c)
				)){
					return false;
			}

		} while(false);

		return true;
	}
//=========================================================================
// findLeftMostChild, findRightMostChild
//=========================================================================
	bool testTraverse(){
		anode<int, int>* a = new anode<int, int>(10,10);
		anode<int, int>* b = new anode<int, int>(11,11);
		anode<int, int>* c = new anode<int, int>(12,12);
		anode<int, int>* d = new anode<int, int>(13,13);
		anode<int, int>* e = new anode<int, int>(14,14);

		anode<int, int>* eee = new anode<int, int>(141,141);

		anode<int, int>* root=NULL;

		if (avl.findLeftMostChild(a) != NULL && avl.findRightMostChild(a) != NULL){
			return false;
		}

		a->left = b;
		a->right = c;
		if (avl.findLeftMostChild(a) != &(a->left)){
			return false;
		}
		if (avl.findRightMostChild(a) != &(a->right)){
			return false;
		}

		b->left = d;
		b->right = e;
		if (avl.findLeftMostChild(a) != &(b->left)){
			return false;
		}
		b->left = NULL;
		b->right = NULL;

		c->left = d;
		c->right = e;
		if (avl.findRightMostChild(a) != &(c->right)){
			return false;
		}
		c->left = NULL;

		e->right = d;
		d->right = eee;

		if (avl.findRightMostChild(a) != &(d->right)){
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
		stack< anode <int, int> ** > paa;
		anode<int, int>** pa = &a->left;
		if (pa != avl.findNodePtrPtr(&a,paa,9, result)){
			return false;
		}

		c->left = eee;
		c->left->left = d;

		pa = &a->right;

		if (pa != avl.findNodePtrPtr(&a, paa, 13, result)){
			return false;
		}

		c->right = e;
		pa = &c->right;
		if (pa != avl.findNodePtrPtr(&a, paa, 14, result)){
			return false;
		}

		pa = &c->left;

		if (pa != avl.findNodePtrPtr(&a, paa, 12, result) ){
			return false;
		}

		return true;
	}

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
	static void createDotFile(const TTree * const tree, const char* fileName){
		FILE*f=fopen(fileName, "wt");
		if (f != NULL){
			char temp[256] = {0};
			int j = 0;
			for(int i = 0; i < strlen(fileName); ++i) if(fileName[i] != '.') temp[j++] = fileName[i];
			if (j == 0) temp[j] = 'G';
			fprintf(f, "digraph %s{\n", temp);
			TRoot* root = tree->m_root;
			createRecord<TRoot>(root, f);
			fprintf(f, "}");
		}
		fclose(f);
	}
private:
	template <typename TRoot>
	static void createRecord(const TRoot* const root, FILE*file){
		if (root != NULL && file != NULL){
			if (root->right != NULL) 
				fprintf(file,"A%d->A%d;\n", root->key, root->right->key);
			if (root->left != NULL)
				fprintf(file,"A%d->A%d;\n", root->key, root->left->key);

			if(root->right != NULL)
				createRecord<TRoot>(root->right, file);
			
			if (root->left != NULL)
				createRecord<TRoot>(root->left, file);
		}
	}
};

#endif