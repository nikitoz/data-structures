#ifndef __FF_AVLTREE__
#define __FF_AVLTREE__
#include <functional>
#include <xmemory>

#ifdef _DEBUG
#include <complex>
#endif // _DEBUG
#pragma warning(disable:4267)

/*
	Internal type for saving key-value pair data of the AVL-tree
*/
class FF_AVLTree_test;
class FF_AVL_GraphCreator;
namespace ff {
namespace {
template
<
	  typename TKey
	, typename TVal
>
struct node {
	node(TKey k = TKey(), TVal v = TVal(), node* p = NULL) 
		: balance(0)
		, right(NULL)
		, left(NULL)
		, childCount(0)
		, parent(p)
		, key(k)
		, val(v)
	{ }

	node(const node& a)
	{ *this = a; }

	const node& operator=(const node& node) {
		key	    = node.key; 
		val	    = node.val; 
		balance = node.balance;
		left    = node.left;
		right   = node.right;
		parent  = node.parent;
		return node;
	}
	
	void update() {
		update_child_count();
		update_balance();
	}

	void update_balance()
	{ balance = (left == NULL ? 0 : left->childCount+1) - (right == NULL ? 0 : right->childCount+1); }

	void update_child_count()
	{ childCount = (left == NULL ? 0 : left->childCount + 1) + (right == NULL ? 0 : right->childCount + 1); }

	TKey key;
	TVal val;
	int balance;
	unsigned int childCount;

	node* parent;
	node* left;
	node* right;
};
}
/*
	Map implementation based on AVL Tree
*/
template
<
	  typename TKey
	, typename TVal
	, typename TPred  = std::less<TKey>
	, typename TAlloc = std::allocator< node<TKey, TVal> > 
 >
class amap {
public :
	typedef node<TKey, TVal> node_t;
	typedef node<TKey, TVal>*ptr_node_t;

	amap()
		: root_(NULL)
		, size_()
	{ }

	const TVal* nodePtr(const TKey& key) const 
	{ return z_find_node(root_, key); }

	void insert(const TKey& key, const TVal& val) 
	{ operator[](key) = val; }

	TVal& operator[](const TKey& key) {
		ptr_node_t* stack_path[StackSize];
		size_t N = 0;
		ptr_node_t* node = z_find_node_ptr_ptr(&root_, key, stack_path, N);
		if (*node)
			return (*node)->val;
		
		*node = alloc(key, TVal(), N ? *stack_path[N-1] : NULL);
		TVal& retval = (*node)->val;
		z_balance_stack(stack_path, N);
		size_++;
		return retval;
	}

	size_t count() const
	{ return size_; }

	bool erase(const TKey& key) {
		if(!root_)
			return false;

		ptr_node_t* stack_for_key[StackSize] = {0};
		size_t N1 = 0;
		ptr_node_t* node_to_remove = z_find_node_ptr_ptr(&root_, key, stack_for_key, N1);

		if (!node_to_remove || !*node_to_remove)
			return false;

		ptr_node_t* stack_path_for_substitution[StackSize];
		size_t N2 = 0;
		ptr_node_t* pp_substitution      = z_find_substitution(*node_to_remove, stack_path_for_substitution, N2);
		ptr_node_t  p_substitution       = pp_substitution ? *pp_substitution : NULL;
		ptr_node_t  p_substitution_child = p_substitution  ?  p_substitution->left ? p_substitution->left : p_substitution->right : NULL;
		ptr_node_t  p_delete_me = *node_to_remove;

		if (pp_substitution) {
			*pp_substitution = p_substitution_child;
			*node_to_remove   = p_substitution;
			p_substitution->left   = p_delete_me->left  != p_substitution ? p_delete_me->left  : NULL;
			p_substitution->right  = p_delete_me->right != p_substitution ? p_delete_me->right : NULL;
			p_substitution->parent = p_delete_me->parent;

			z_update(p_substitution->left);
			z_update(p_substitution->right);
			z_update(p_substitution);

		} else *node_to_remove = NULL;

		for (size_t i = N2-1; i != UNDER_ZERO; --i)
			z_update(*(stack_path_for_substitution[i]));

		z_balance_stack(stack_for_key, N1);
		dealloc(p_delete_me);
		--size_;
		return true;
	}

	/*
		Nested iterator class
		Supports -> and ++ (TODO: --)
	*/
	struct iterator {
		iterator(ptr_node_t _node)
		{ node_ = _node; }

		iterator()
		{ node_ = NULL; }

		iterator(const iterator& _i)
		{ node_ = _i.node_; }

		const iterator& operator++() {
			// Node is visited go to left-visit-right
			ptr_node_t prev = node_;
			if (node_->right) {
				node_ = node_->right;
				while(node_->left) 
					node_ = node_->left;
				return *this;
			}

			node_ = node_->parent;

			if (!node_)
				return *this;// end()

			// if prev was left then visit this node
			if (node_->left == prev)
				return *this; // then go to right

			// if prev was right then go up, until we're not in right subtree
			while(node_ && node_->right == prev) {
				prev = node_;
				node_ = node_->parent;
			}
			return *this;
		}

		const iterator& operator--() {
			// TODO : implement 
		}

		bool operator!=(const iterator& i) const 
		{ return node_ != i.node_; }

		const ptr_node_t operator->() const 
		{ return node_; }

	private:
		ptr_node_t node_;
	};// iterator

	//This intended to work in O(log n)
	iterator begin() {
		ptr_node_t current = root_;
		while(current->left) current = current->left;
		return iterator(current);
	}

	iterator end() {
		return iterator();
	}

protected:

	inline void z_balance_stack(ptr_node_t** nodes, size_t& N) {
		ptr_node_t* node;
		for (size_t i = N - 1; i != UNDER_ZERO; --i) {
			node = nodes[i];
			*node = z_balance(*node);
		}
	}

	inline void z_update(ptr_node_t node) {
		if (!node) return;
		node->update_child_count();
		node->update_balance();
		if (node->left) node->left->parent  = node;
		if (node->right)node->right->parent = node;
	}

	inline void z_update_after_rotate(ptr_node_t a, ptr_node_t b) {
		a->update_child_count();
		b->update_child_count();
		a->update_balance();
		b->update_balance();
		b->parent = a->parent;
		a->parent = b;
		if (a->left)  a->left->parent = a;
		if (a->right) a->right->parent = a;
		if (b->left)  b->left->parent = b;
		if (b->right) b->right->parent = b;
	}
	/*
	a         b
	 \  =>   /
	  b     a
	*/
	inline ptr_node_t z_single_left_rotate(ptr_node_t a) {
		ptr_node_t b = a->right;
		a->right = b->left;
		b->left = a;
		z_update_after_rotate(a, b);
		return b;
	}
	/*
	  a    b
	 /  =>  \
	b	     a
	*/
	inline ptr_node_t z_single_right_rotate(ptr_node_t a) {
		ptr_node_t b = a->left;
		a->left = b->right;
		b->right = a;
		z_update_after_rotate(a, b);
		return b;
	}

	inline ptr_node_t z_double_left_rotate(ptr_node_t a) {
		ptr_node_t b = a->right;
		a->right = z_single_right_rotate(b);
		return z_single_left_rotate(a);
	}

	inline ptr_node_t z_double_right_rotate(ptr_node_t a) {
		ptr_node_t b = a->left;
		a->left = z_single_left_rotate(b);
		return z_single_right_rotate(a);
	}

	ptr_node_t* z_find_substitution(ptr_node_t& root, ptr_node_t** parents,  size_t& N) {
		ptr_node_t* pRetVal;
		pRetVal = getOnlyOneChild(root);
		if (pRetVal)
			return pRetVal;

		if (root->left)
			return z_find_very_right_child(root->left, parents, N);

		if (root->right)
			return z_find_very_left_child(root->right, parents, N);

		return NULL;
	}

	ptr_node_t* getOnlyOneChild(ptr_node_t root) {
		if (root && root->childCount == 1)
			return root->left ?  &(root->left) : &(root->right);
		return NULL;
	}

	ptr_node_t* z_find_very_left_child(ptr_node_t& root, ptr_node_t** parents,  size_t& N) {
		ptr_node_t* node = &root;
		size_t i = 0;
		while ((*node)->left) {
			parents[i++] = node;
			node = &((*node)->left);
		}
		parents[i++] = node;
		N = i;

		return node;
	}

	ptr_node_t* z_find_very_right_child(ptr_node_t& root, ptr_node_t** parents,  size_t& N) {
		ptr_node_t* node = &root;
		size_t i = 0;
		while ((*node)->right) {
			parents[i++] = node;
			node = &((*node)->right);
		}
		parents[i++] = node;
		N = i;

		return node;
	}

	ptr_node_t* z_find_node_ptr_ptr(ptr_node_t* root, const TKey& key, ptr_node_t** parents, size_t& N) {
		ptr_node_t* node = root;

		size_t i = 0;
		while(*node && (*node)->key != key) {
			parents[i++] = node;
			node = z_p_child_on_predicate(*node, key);
		}
		N = i;
		return node;
	}

	const TVal* z_find_node(const ptr_node_t root, const TKey& key) const {
		ptr_node_t current = root;

		while (current && current->key != key)
			current = z_child_on_predicate(current, key);
		
		return current ? &(current->val) : NULL;
	}

	inline const ptr_node_t& z_child_on_predicate(const ptr_node_t root, const TKey& key) const {
		return (predicate_(root->key, key)) ? root->right : root->left;
	}

	inline ptr_node_t* z_p_child_on_predicate(const ptr_node_t root, const TKey& key) {
		return (predicate_(root->key, key)) ? &(root->right) : &(root->left);
	}

	inline ptr_node_t z_balance(ptr_node_t root) {
		z_update(root->left);
		z_update(root->right);
		z_update(root);

		if (root->balance >= 2) {
			ptr_node_t left = root->left;
			if(left->balance >= 0)
				return z_single_right_rotate(root);
			if (left->balance <= -1)
				return z_double_right_rotate(root);
			
		} else if (root->balance <= -2) {
			ptr_node_t right = root->right;
			if (right->balance <= 0)
				return z_single_left_rotate(root);

			if (right->balance >= 1) 
				return z_double_left_rotate(root);
		}
		return root;
	}

	inline void dealloc(ptr_node_t node) {
		if(node)
			allocator_.deallocate(node, 1);
		node = NULL;
	}

	inline ptr_node_t alloc(const TKey& key, const TVal& val, ptr_node_t parent) {
		ptr_node_t ptr = allocator_.allocate(1,0);
		allocator_.construct(ptr, node_t(key, val, parent));
		return ptr;
	}

	amap(const amap& _amap) 
	{/*Not available*/}
	const amap& operator=(const amap& _amap)
	{/*Not available*/}


	ptr_node_t root_;
	TPred	 predicate_;
	TAlloc	 allocator_;
	size_t   size_;
	enum {StackSize = 4096};
	static const size_t UNDER_ZERO = (size_t)(0-1);

#ifdef _DEBUG
public:
	// Friend class for testing private member functions
	friend class ::FF_AVLTree_test;
	// Friend class for creating dot files
	friend class ::FF_AVL_GraphCreator;

	/*
		Next methods may be used only for tests.
		They are VERY inefficient and ugly, don't look at them
	*/
	int get_max_path(ptr_node_t root) {
		if (root != NULL) {
			return std::max<int>(get_max_path(root->left)+1, get_max_path(root->right) + 1); 
		} else { 
			return 0;
		}
	}

	int get_num_elements(const ptr_node_t root) const {
		if (!root)
			return 0;
		return get_num_elements(root->left) + get_num_elements(root->right) + 1;
	}

	bool checkParents_internal(ptr_node_t node, ptr_node_t parent) {
		if (!node)
			return true;

		if (node->parent != parent)
			return false;

		return checkParents_internal(node->left, node) &&
			   checkParents_internal(node->right, node);
	}

	bool check_child_counts(ptr_node_t root) {
		if (!root)
			return true;
		int ileft = get_num_elements(root->left);
		int iright = get_num_elements(root->right);
		if (ileft + iright  == root->childCount)
			if (check_child_counts(root->left) && check_child_counts(root->right))
				return true;
		return false;
	}

	// returns true if this tree balance factor is less than 1.44*(log n)
	bool check_balancing(int elements) {
		int n = get_max_path(root_);
		double nom = (double)log10((double)elements);
		double denom = (double)log10((double)2);
		int k = (long)ceil(1.44*nom/denom);
		if ((n - k) <= 1)
			return true;
		else  
			return false;
	}
	// returns maximum path in a tree
	int max_path() 
	{ return get_max_path(root_); }
	// returns amount of elements in a tree
	int dcount() const 
	{ return get_num_elements(root_); }
	// returns true if parent property is correct in every node
	bool check_parents() 
	{ return checkParents_internal(root_, NULL); }
	// returns true if childCount property is set correctly in every node
	bool check_children() 
	{ return check_child_counts(root_); }

#endif//_DEBUG
};
} // namespace ff
#endif//__FF_AVLTREE__
