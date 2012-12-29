#include "AVLTree_test.h"
#ifndef __FF_AVL_TREE__
#define __FF_AVL_TREE__

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
#include <cstdlib>
#include <functional>
#include <cmath>
#include <memory.h>
#include <exception>

namespace {
	//=========================================================================
	// Internal type for saving node data of the AVL-tree
	//=========================================================================
	template<typename TKey, typename TVal>
	struct anode {
		anode()
			: balance(0)
			, right(NULL)
			, left(NULL)
			, childCount(0) {}

		anode(TKey _k, TVal _v) 
			: balance(0)
			, right(NULL)
			, left(NULL)
			, childCount(0)
		{ key = _k; val = _v; }

		anode(const anode<TKey, TVal>& a)
		{ (*this) = a; }

		const anode& operator=(const anode& _node) {
			this->key	  = _node.key; 
			this->val	  = _node.val; 
			this->balance = _node.balance;
			return _node;
		}

		void calcBalance() {
			balance = ((left == NULL)? 0 : left->childCount+1) - ((right == NULL)? 0 : right->childCount+1);
		}

		void calcChildCount() {
			childCount = ((left == NULL)? 0 : left->childCount + 1) + ((right == NULL)? 0 : right->childCount + 1);
		}

		TKey key;
		TVal val;
		int balance;
		int childCount;

		anode<TKey, TVal> * left;
		anode<TKey, TVal> * right;
	};
}
//=========================================================================
// Map implementation based on AVL Tree
//=========================================================================
template <typename TKey, typename TVal, typename TPred = std::less<TKey> >
class ff_amap {
public :
	typedef anode<TKey, TVal>  anode_t;
	typedef anode<TKey, TVal>* panode_t;
	typedef std::stack<panode_t*>   stack_panode_t;

private:
	panode_t m_root;
	TPred m_predicate;

	enum {StackSize = 4096};

public:
	ff_amap()
		: m_root(NULL) {}

	// Friend class for testing private member functions
	friend class FF_AVLTree_test;
	// Friend class for creating dot files
	friend class FF_AVL_GraphCreator;

	//=========================================================================
	bool erase(const TKey& key) {
		if(!m_root)
			return false;

		panode_t* stackPathForKey[StackSize] = {0};
		int N1 = 0;
		panode_t* nodeToRemove = (m_root->key == key) ? &m_root : findNodePtrPtr(&m_root, key, stackPathForKey, N1);

		if (!nodeToRemove || (!*nodeToRemove))
			return false;

		panode_t* stackPathForSub[StackSize];
		int N2 = 0;
		panode_t* ppSubstitution = findSubstitution(*nodeToRemove, stackPathForSub, N2);
		panode_t  pSubstitution = ppSubstitution ? *ppSubstitution : NULL;
		panode_t pSubstitutionChild = pSubstitution ? pSubstitution->left ? pSubstitution->left : pSubstitution->right : NULL;
		panode_t pDeleteMe = (*nodeToRemove);

		if (ppSubstitution) {
			*ppSubstitution = pSubstitutionChild;
			*nodeToRemove   = pSubstitution;
			pSubstitution->left  = pDeleteMe->left  != pSubstitution ? pDeleteMe->left  : NULL;
			pSubstitution->right = pDeleteMe->right != pSubstitution ? pDeleteMe->right : NULL;
			updateNode(pSubstitution->left);
			updateNode(pSubstitution->right);
			updateNode(pSubstitution);
			
		} else {
			*nodeToRemove = NULL;
		}

		for (int i = N2-1; i >= 0; --i)
			updateNode(*(stackPathForSub[i]));

		balanceStack(stackPathForKey, N1);
		deallocNode(pDeleteMe);
		return true;
	}
	//=========================================================================
	const TVal& findNode(const TKey& key) const  {
		return operator[](key);
	}
	//=========================================================================
	const TVal* findPtrToNode(const TKey& key) const {
		return findNodeInt(m_root, key);
	}
	//=========================================================================
	const TVal& operator[](const TKey& key) const {
		const TVal* val = findNodeInt(m_root, key);

		if (val)
			return *val;

		return TVal();
	}
	//=========================================================================
	TVal& operator[](const TKey& key) {
		panode_t* stackPath[StackSize];
		int N = 0;
		panode_t* pNode = findNodePtrPtr(&m_root, key, stackPath, N);
		if (*pNode)
			return (*pNode)->val;
		
		*pNode = allocNode(key, TVal());
		TVal& retVal = (*pNode)->val;
		balanceStack(stackPath, N);
		return retVal;
	}
	//=========================================================================
	void insertNode(const TKey& key, const TVal& val) {
		int N = 0;
		insertInt(&m_root, key, val, N);
		balanceStack(m_parentStack, N);
	}

private:
	//=========================================================================
	inline void balanceStack(panode_t** nodes, int &N) {
		panode_t* node;
		for (int i = N - 1; i >= 0; --i) {
			node = nodes[i];
			*node = balanceSubtree(*node);
		}
	}
	//=========================================================================
	inline void updateNode(panode_t node) {
		if (!node)
			return;
		node->calcChildCount();
		node->calcBalance();
	}
	//=========================================================================
	inline void updateTwoNodesCnt(panode_t a, panode_t b) {
		a->calcChildCount();
		b->calcChildCount();
		a->calcBalance();
		b->calcBalance();
	}
	//=========================================================================
	//a         b
	// \  =>   /
	//	b     a
	//=========================================================================
	inline panode_t littleLeftRotate(panode_t a) {
		panode_t b = a->right;
		a->right = b->left;
		b->left = a;
		updateTwoNodesCnt(a, b);
		return b;
	}
	//=========================================================================
	//  a    b
	// /  =>  \
	//b	       a
	//=========================================================================
	inline anode_t* littleRightRotate(panode_t a) {
		panode_t b = a->left;
		a->left = b->right;
		b->right = a;
		updateTwoNodesCnt(a, b);
		return b;
	}
	//=========================================================================
	inline panode_t bigLeftRotate(panode_t a) {
		panode_t b = a->right;
		a->right = littleRightRotate(b);
		return littleLeftRotate(a);
	}
	//=========================================================================
	inline panode_t bigRightRotate(panode_t a) {
		panode_t b = a->left;
		a->left = littleLeftRotate(b);
		return littleRightRotate(a);
	}
	//=========================================================================
	panode_t* findSubstitution(panode_t& root, panode_t** parents,  int &N) {
		if (!root)
			return NULL;

		panode_t* pRetVal;
		pRetVal = getOnlyOneChild(root);
		if (pRetVal)
			return pRetVal;

		if (root->left)
			return findRightMostChild(root->left, parents, N);

		if (root->right)
			return findLeftMostChild(root->right, parents, N);

		return NULL;
	}
	//=========================================================================
	panode_t* getOnlyOneChild(panode_t root) {
		if (!root)
			return NULL;
		if (root && root->childCount == 1)
			return root->left ?  &(root->left) : &(root->right);
		return NULL;
	}
	//=========================================================================
	panode_t* findLeftMostChild(panode_t& root, panode_t** parents,  int &N) {
		panode_t* node = &root;
		int i = 0;
		while ((*node)->left) {
			parents[i++] = node;
			node = &((*node)->left);
		}
		parents[i++] = node;
		N = i;

		return node;
	}
	//=========================================================================
	panode_t* findRightMostChild(panode_t& root, panode_t** parents,  int &N) {
		panode_t* node = &root;
		int i = 0;
		while ((*node)->right) {
			parents[i++] = node;
			node = &((*node)->right);
		}
		parents[i++] = node;
		N = i;

		return node;
	}
	//=========================================================================
	panode_t* findNodePtrPtr(panode_t* root, const TKey& key, panode_t** parents, int& N) {
		panode_t* node = root;

		int i = 0;
		while(*node && (*node)->key != key) {
			parents[i++] = node;
			node = pchildOnPredicate(*node, key);
		}
		N = i;
		return node;
	}
	//=========================================================================
	TVal* findNodeInt(const panode_t root, const TKey& key) const {
		if (!root)
			return NULL;

		if (root->key == key)
			return &(root->val);

		return findNodeInt(childOnPredicate(root, key), key);
	}
	//=========================================================================
	inline const panode_t& childOnPredicate(const panode_t root, const TKey& key) const {
		return (m_predicate(root->key, key)) ? root->right : root->left;
	}
	//=========================================================================
	inline panode_t* pchildOnPredicate(const panode_t root, const TKey& key) {
		return (m_predicate(root->key, key)) ? &(root->right) : &(root->left);
	}
	//=========================================================================
	inline panode_t& childOnPredicate(panode_t root, const TKey& key) {
		return (m_predicate(root->key, key)) ? root->right : root->left;
	}
	//=========================================================================
	// takes the root of the subtree to balance, returns new root
	//=========================================================================
	inline panode_t balanceSubtree(panode_t root) {
		updateNode(root->left);
		updateNode(root->right);
		updateNode(root);

		if (root->balance >= 2) {
			panode_t left = root->left;
			if(left->balance >= 0)
				return littleRightRotate(root);
			if (left->balance <= -1)
				return bigRightRotate(root);
			
		} else if (root->balance <= -2) {
			panode_t right = root->right;
			if (right->balance <= 0)
				return littleLeftRotate(root);

			if (right->balance >= 1) 
				return bigLeftRotate(root);
		}
		return root;
	}
	//=========================================================================
	// Deallocate node
	//=========================================================================
	inline void deallocNode(panode_t node) {
		if(node)
			delete node;
		node = NULL;
	}
	//=========================================================================
	inline panode_t allocNode(const TKey& key, const TVal& val) {
		return new anode_t(key, val);
	}
	//=========================================================================
	// Returns length of the max path in the tree. !Very inefficient! Only for tests
	//=========================================================================
	int getMaxPath(panode_t root) {
		if (root != NULL) {
			return std::max<int>(getMaxPath(root->left)+1, getMaxPath(root->right) + 1); 
		} else { 
			return 0;
		}
	}
	//=========================================================================
	int getNumElements(const panode_t root) const {
		if (!root)
			return 0;
		return getNumElements(root->left) + getNumElements(root->right) + 1;
	}

public:
	//=========================================================================
	// Check balance factor of the tree, pass amount of elements
	//=========================================================================
	bool checkBalancing(int elements) {
		int n = getMaxPath(m_root);
		double nom = (double)log10((double)elements);
		double denom = (double)log10((double)2);
		int k = (long)ceil(1.44*(double)log10((double)elements) / (double)log10((double)2));
		if ((n - k) <= 1)
			return true;
		else  
			return false;
	}
	//=========================================================================
	// returns maximum path in a tree
	//=========================================================================
	int maxPath() {
		return getMaxPath(m_root);
	}
	//=========================================================================
	// returns amount of elements in a tree
	//=========================================================================
	int count() const {
		return getNumElements(m_root);
	}
	//=========================================================================
	bool checkChildCounts(panode_t root) {
		if (!root)
			return true;
		int ileft = getNumElements(root->left);
		int iright = getNumElements(root->right);
		if (ileft + iright  == root->childCount) {
			if (checkChildCounts(root->left) && checkChildCounts(root->right)) {
				return true;
			}
		} 
		return false;
	}
	//=========================================================================
	bool checkChildren() {
		return checkChildCounts(m_root);
	}
};
#endif