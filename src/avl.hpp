#include "_tests/AVLTree_test.h"
#ifndef __FF_AVLTREE__
#define __FF_AVLTREE__
#include <functional>
#include <xmemory>

#ifdef _DEBUG
#include <complex>
#endif // _DEBUG
#pragma warning(disable:4267)
//=========================================================================
// Internal type for saving node data of the AVL-tree
//=========================================================================
namespace {
	template<typename TKey, typename TVal>
	struct anode {
		anode()
			: balance(0)
			, right(NULL)
			, left(NULL)
			, childCount(0)
			, parent(NULL) {}

		anode(TKey _k, TVal _v, anode* _parent) 
			: balance(0)
			, right(NULL)
			, left(NULL)
			, childCount(0)
		{ key = _k; val = _v; parent = _parent; }

		anode(TKey _k, TVal _v) // Deprecated
			: balance(0)
			, right(NULL)
			, left(NULL)
			, childCount(0)
			, parent(NULL)
		{ key = _k; val = _v; }

		anode(const anode& a)
		{ (*this) = a; }

		const anode& operator=(const anode& _node) {
			this->key	  = _node.key; 
			this->val	  = _node.val; 
			this->balance = _node.balance;
			this->left    = _node.left;
			this->right   = _node.right;
			this->parent  = _node.parent;
			return _node;
		}

		void calcBalance()
		{ balance = ((left == NULL)? 0 : left->childCount+1) - ((right == NULL)? 0 : right->childCount+1); }

		void calcChildCount()
		{ childCount = ((left == NULL)? 0 : left->childCount + 1) + ((right == NULL)? 0 : right->childCount + 1); }

		TKey key;
		TVal val;
		int balance;
		size_t childCount;

		anode * parent;
		anode * left;
		anode * right;
	};

	static void ff_assert(bool expression, const char* text = NULL) {
#ifdef _DEBUG
		if (! expression )
			throw std::exception(text);
#endif
	}
}
#pragma warning(default:4267)
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

	ff_amap()
		: m_root(NULL)
		, m_size() {}


	bool erase(const TKey& key) {
		if(!m_root)
			return false;

		panode_t* stackPathForKey[StackSize] = {0};
		size_t N1 = 0;
		panode_t* nodeToRemove = findNodePtrPtr(&m_root, key, stackPathForKey, N1);

		if (!nodeToRemove || !*nodeToRemove)
			return false;

		panode_t* stackPathForSub[StackSize];
		size_t N2 = 0;
		panode_t* ppSubstitution = findSubstitution(*nodeToRemove, stackPathForSub, N2);
		panode_t  pSubstitution = ppSubstitution ? *ppSubstitution : NULL;
		panode_t pSubstitutionChild = pSubstitution ? pSubstitution->left ? pSubstitution->left : pSubstitution->right : NULL;
		panode_t pDeleteMe = *nodeToRemove;

		if (ppSubstitution) {
			*ppSubstitution = pSubstitutionChild;
			*nodeToRemove   = pSubstitution;
			pSubstitution->left  = pDeleteMe->left  != pSubstitution ? pDeleteMe->left  : NULL;
			pSubstitution->right = pDeleteMe->right != pSubstitution ? pDeleteMe->right : NULL;
			pSubstitution->parent = pDeleteMe->parent;

			updateNode(pSubstitution->left);
			updateNode(pSubstitution->right);
			updateNode(pSubstitution);
			
		} else {
			*nodeToRemove = NULL;
		}

		for (size_t i = N2-1; i != UNDER_ZERO; --i)
			updateNode(*(stackPathForSub[i]));

		balanceStack(stackPathForKey, N1);
		dealloc(pDeleteMe);
		m_size--;
		return true;
	}

	const TVal* nodePtr(const TKey& key) const 
	{ return findNodeInt(m_root, key); }

	void insert(const TKey& key, const TVal& val) 
	{ operator[](key) = val; }

	TVal& operator[](const TKey& key) {
		panode_t* stackPath[StackSize];
		size_t N = 0;
		panode_t* pNode = findNodePtrPtr(&m_root, key, stackPath, N);
		if (*pNode)
			return (*pNode)->val;
		
		*pNode = alloc(key, TVal(), N? *stackPath[N-1] : NULL);
		TVal& retVal = (*pNode)->val;
		balanceStack(stackPath, N);
		m_size++;
		return retVal;
	}

	size_t count() const
	{ return m_size; }

	//=========================================================================
	// Nested iterator class
	// Supports -> and ++ (TODO: --)
	//=========================================================================
	class iterator {
	public:
		iterator(panode_t _node)
		{ m_node = _node; }

		iterator()
		{ m_node = NULL; }

		iterator(const iterator& _i)
		{ m_node = _i.m_node; }

		const iterator& operator++() {
			// Node is visited go to left-visit-right
			panode_t prev = m_node;
			if (m_node->right) {
				m_node = m_node->right;
				while(m_node->left) 
					m_node = m_node->left;
				return *this;
			}

			m_node = m_node->parent;

			if (!m_node)
				return *this;// end()

			// if prev was left then visit this node
			if (m_node->left == prev) {
				return *this; // then go to right
			}

			// if prev was right then go up, until we're not in right subtree
			if (m_node->right == prev) {
				while(m_node && m_node->right == prev) {
					prev = m_node;
					m_node = m_node->parent;
				}
				return *this;
			}

			return *this;
		}

		const iterator& operator--() {
			// TODO : 
		}

		bool operator!=(const iterator& comp2) const 
		{ return this->m_node != comp2.m_node; }

		const panode_t operator->() const 
		{ return m_node; }

	private:
		panode_t m_node;
	};// iterator

	//=========================================================================
	// This intended to work in O(log n)
	//=========================================================================
	iterator begin() {
		panode_t current = m_root;
		while(current->left) current = current->left;
		return iterator(current);
	}

	iterator end() {
		return iterator();
	}

private:
	//=========================================================================
	//  PRIVATE
	//=========================================================================

	inline void balanceStack(panode_t** nodes, size_t& N) {
		panode_t* node;
		for (size_t i = N - 1; i != UNDER_ZERO; --i) {
			node = nodes[i];
			*node = balanceSubtree(*node);
		}
	}

	inline void updateNode(panode_t node) {
		if (!node)
			return;
		node->calcChildCount();
		node->calcBalance();
		if (node->left) 
			node->left->parent = node;
		if (node->right) 
			node->right->parent = node;
	}

	inline void updateTwoNodesAfterRotate(panode_t a, panode_t b) {
		a->calcChildCount();
		b->calcChildCount();
		a->calcBalance();
		b->calcBalance();
		b->parent = a->parent;
		a->parent = b;
		if (a->left)  a->left->parent = a;
		if (a->right) a->right->parent = a;
		if (b->left)  b->left->parent = b;
		if (b->right) b->right->parent = b;
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
		updateTwoNodesAfterRotate(a, b);
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
		updateTwoNodesAfterRotate(a, b);
		return b;
	}

	inline panode_t bigLeftRotate(panode_t a) {
		panode_t b = a->right;
		a->right = littleRightRotate(b);
		return littleLeftRotate(a);
	}

	inline panode_t bigRightRotate(panode_t a) {
		panode_t b = a->left;
		a->left = littleLeftRotate(b);
		return littleRightRotate(a);
	}

	panode_t* findSubstitution(panode_t& root, panode_t** parents,  size_t& N) {
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

	panode_t* getOnlyOneChild(panode_t root) {
		if (root && root->childCount == 1)
			return root->left ?  &(root->left) : &(root->right);
		return NULL;
	}

	panode_t* findLeftMostChild(panode_t& root, panode_t** parents,  size_t& N) {
		panode_t* node = &root;
		size_t i = 0;
		while ((*node)->left) {
			parents[i++] = node;
			node = &((*node)->left);
		}
		parents[i++] = node;
		N = i;

		return node;
	}

	panode_t* findRightMostChild(panode_t& root, panode_t** parents,  size_t& N) {
		panode_t* node = &root;
		size_t i = 0;
		while ((*node)->right) {
			parents[i++] = node;
			node = &((*node)->right);
		}
		parents[i++] = node;
		N = i;

		return node;
	}

	panode_t* findNodePtrPtr(panode_t* root, const TKey& key, panode_t** parents, size_t& N) {
		panode_t* node = root;

		size_t i = 0;
		while(*node && (*node)->key != key) {
			parents[i++] = node;
			node = pchildOnPredicate(*node, key);
		}
		N = i;
		return node;
	}

	const TVal* findNodeInt(const panode_t root, const TKey& key) const {
		panode_t current = root;

		while (current && current->key != key)
			current = childOnPredicate(current, key);
		
		return current ? &(current->val) : NULL;
	}

	inline const panode_t& childOnPredicate(const panode_t root, const TKey& key) const {
		return (m_predicate(root->key, key)) ? root->right : root->left;
	}

	inline panode_t* pchildOnPredicate(const panode_t root, const TKey& key) {
		return (m_predicate(root->key, key)) ? &(root->right) : &(root->left);
	}

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

	inline void dealloc(panode_t node) {
		if(node)
			m_allocator.deallocate(node, 1);
		node = NULL;
	}

	inline panode_t alloc(const TKey& key, const TVal& val, panode_t parent) {
		panode_t ptr = m_allocator.allocate(1,0);
		m_allocator.construct(ptr, anode_t(key, val, parent));
		return ptr;
	}

	ff_amap(const ff_amap& _amap) 
	{/*Not available*/}
	const ff_amap& operator=(const ff_amap& _amap)
	{/*Not available*/}


	panode_t m_root;
	TPred	 m_predicate;
	TAlloc	 m_allocator;
	size_t   m_size;
	enum {StackSize = 4096};
	static const size_t UNDER_ZERO = (size_t)(0-1);

#ifdef _DEBUG

	// Friend class for testing private member functions
	friend class FF_AVLTree_test;
	// Friend class for creating dot files
	friend class FF_AVL_GraphCreator;

//=========================================================================
// Next methods may be used only for tests.
// They are VERY inefficient and ugly, don't look at them
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
	//=========================================================================
	bool checkParents_internal(panode_t node, panode_t parent) {
		if (!node)
			return true;

		if (node->parent != parent)
			return false;

		return checkParents_internal(node->left, node) &&
			   checkParents_internal(node->right, node);
	}
	//=========================================================================
	bool checkChildCounts(panode_t root) {
		if (!root)
			return true;
		int ileft = getNumElements(root->left);
		int iright = getNumElements(root->right);
		if (ileft + iright  == root->childCount)
			if (checkChildCounts(root->left) && checkChildCounts(root->right))
				return true;
		return false;
	}

public:
	//=========================================================================
	// returns true if this tree balance factor is less than 1.44*(log n)
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
	int maxPath() 
	{ return getMaxPath(m_root); }
	//=========================================================================
	// returns amount of elements in a tree
	//=========================================================================
	int dcount() const 
	{ return getNumElements(m_root); }
	//=========================================================================
	// returns true if parent property is correct in every node
	//=========================================================================
	bool checkParents() 
	{ return checkParents_internal(m_root, NULL); }
	//=========================================================================
	// returns true if childCount property is set correctly in every node
	//=========================================================================
	bool checkChildren() 
	{ return checkChildCounts(m_root); }

#endif//_DEBUG
};
#endif//__FF_AVLTREE__