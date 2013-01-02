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
			this->left    = _node.left;
			this->right   = _node.right;
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

	static void ff_assert(bool expression, const char* text = NULL) {
#ifdef _DEBUG
		if (! expression )
			throw std::exception(text);
#endif
	}
}
//=========================================================================
// Map implementation based on AVL Tree
//=========================================================================
template <typename TKey
, typename TVal
, typename TPred = std::less<TKey>
, typename TAlloc = std::allocator<anode<TKey, TVal> > >
class ff_amap {
public :
	typedef anode<TKey, TVal>	  anode_t;
	typedef anode<TKey, TVal>*	  panode_t;
	typedef std::stack<panode_t*> stack_panode_t;

private:
	panode_t m_root;
	TPred	 m_predicate;
	TAlloc	 m_allocator;

	size_t   m_size;

	enum {StackSize = 4096};

public:
	ff_amap()
		: m_root(NULL)
		, m_size() {}

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
		panode_t* nodeToRemove = findNodePtrPtr(&m_root, key, stackPathForKey, N1);

		if (!nodeToRemove || !*nodeToRemove)
			return false;

		panode_t* stackPathForSub[StackSize];
		int N2 = 0;
		panode_t* ppSubstitution = findSubstitution(*nodeToRemove, stackPathForSub, N2);
		panode_t  pSubstitution = ppSubstitution ? *ppSubstitution : NULL;
		panode_t pSubstitutionChild = pSubstitution ? pSubstitution->left ? pSubstitution->left : pSubstitution->right : NULL;
		panode_t pDeleteMe = *nodeToRemove;

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
		dealloc(pDeleteMe);
		m_size++;
		return true;
	}
	//=========================================================================
	const TVal* nodePtr(const TKey& key) const {
		return findNodeInt(m_root, key);
	}
	//=========================================================================
	void insert(const TKey& key, const TVal& val) {
		operator[](key) = val;
	}
	//=========================================================================
	TVal& operator[](const TKey& key) {
		panode_t* stackPath[StackSize];
		int N = 0;
		panode_t* pNode = findNodePtrPtr(&m_root, key, stackPath, N);
		if (*pNode)
			return (*pNode)->val;
		
		*pNode = alloc(key, TVal());
		TVal& retVal = (*pNode)->val;
		balanceStack(stackPath, N);
		m_size++;
		return retVal;
	}
	//=========================================================================
	size_t count() {
		return m_size;
	}


private:
	//=========================================================================
	//  PRIVATE
	//=========================================================================

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
		ff_assert(root != NULL, "NULL passed to findSubstitution");

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
	const TVal* findNodeInt(const panode_t root, const TKey& key) const {
		panode_t current = root;

		while (current && current->key != key)
			current = childOnPredicate(current, key);
		
		return current ? &(current->val) : NULL;
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
	// alloc/dealloc
	//=========================================================================
	inline void dealloc(panode_t node) {
		if(node)
			m_allocator.deallocate(node, 1);
		node = NULL;
	}
	//=========================================================================
	inline panode_t alloc(const TKey& key, const TVal& val) {
		panode_t ptr = m_allocator.allocate(1,0);
		m_allocator.construct(ptr, anode_t(key, val));
		return ptr;
	}
	//=========================================================================
	// Only for tests
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
	// Test stuff
	//=========================================================================
	bool checkBalancing(int elements) {
		int n = getMaxPath(m_root);
		double nom = (double)log10((double)elements);
		double denom = (double)log10((double)2);
		int k = (long)ceil(1.44*nom/denom);
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
	int dcount() const {
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