#ifndef __FF_AVLTREE__
#define __FF_AVLTREE__
#include <functional>
#include <xmemory>

#ifdef _DEBUG
#include <complex>
#endif // _DEBUG
#pragma warning(disable:4267)
#include "contiguous_stack.hpp"
#include "algorithms.hpp"
#include <math.h>

/*
	Internal type for saving key-value pair data of the AVL-tree
*/
class FF_AVLTree_test;
class FF_AVL_GraphCreator;
namespace ff { namespace {

template
<
	  typename key_t
	, typename value_t
>
struct node {
	node(key_t k = key_t(), value_t v = value_t(), node* p = 0)
		: balance(0)
		, right(0)
		, left(0)
		, childCount(0)
		, parent(p)
		, first(k)
		, second(v)
	{ }

	node(const node& a)
	{ *this = a; }

	const node& operator=(const node& node) {
		first	    = node.first; 
		second	    = node.second; 
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
	{ balance    = (left == 0 ? 0 : left->childCount + 1) - (right == 0 ? 0 : right->childCount + 1); }

	void update_child_count()
	{ childCount = (left == 0 ? 0 : left->childCount + 1) + (right == 0 ? 0 : right->childCount + 1); }

	key_t        first;
	value_t      second;
	int          balance;
	unsigned int childCount;

	node* parent;
	node* left;
	node* right;
}; // node
}  // anonymous-namespace
/*
	Map implementation based on AVL Tree
*/
template
<
	  typename key_t
	, typename value_t
	, typename pred_t  = std::less<key_t>
	, typename alloc_t = std::allocator< node<key_t, value_t> > 
 >
class amap {
public :
	typedef node<key_t, value_t> node_t;
	typedef node<key_t, value_t>*ptr_node_t;
	typedef contiguous_stack<ptr_node_t*> parent_stack_t;
	struct iterator;

	amap()
		: root_(nullptr)
		, size_()
	{ }

	const value_t* nodePtr(const key_t& key) const 
	{ return z_find_node(root_, key); }

	void insert(const key_t& key, const value_t& val) 
	{ operator[](key) = val; }

	value_t& operator[](const key_t& key) {
		parent_stack_t stack_for_key(bound_height());
		ptr_node_t* node = z_find_node_ptr_ptr(&root_, key, stack_for_key);
		if (*node)
			return (*node)->second;
		
		*node = alloc(key, value_t(), stack_for_key.empty() ? nullptr : *stack_for_key.top());
		value_t& retval = (*node)->second;
		z_balance_stack(stack_for_key);
		size_++;
		return retval;
	}

	iterator find(const key_t& key) {
		ptr_node_t* node = z_find_node_ptr_ptr(&root_, key, stack_for_key);
		if (*node)
			return iterator(*node);
		return end();
	}

	size_t count() const
	{ return size_; }

	bool erase(const key_t& key) {
		if(!root_)
			return false;
		parent_stack_t stack_for_key(bound_height());
		ptr_node_t* node_to_remove = z_find_node_ptr_ptr(&root_, key, stack_for_key);

		if (!node_to_remove || !*node_to_remove)
			return false;

		parent_stack_t stack_path_for_substitution(bound_height());
		ptr_node_t* pp_substitution      = z_find_substitution(*node_to_remove, stack_path_for_substitution);
		ptr_node_t  p_substitution       = pp_substitution ? *pp_substitution : nullptr;
		ptr_node_t  p_substitution_child = p_substitution  ?  p_substitution->left ? p_substitution->left : p_substitution->right : nullptr;
		ptr_node_t  p_delete_me = *node_to_remove;

		if (pp_substitution) {
			*pp_substitution = p_substitution_child;
			*node_to_remove   = p_substitution;
			p_substitution->left   = p_delete_me->left  != p_substitution ? p_delete_me->left  : nullptr;
			p_substitution->right  = p_delete_me->right != p_substitution ? p_delete_me->right : nullptr;
			p_substitution->parent = p_delete_me->parent;

			z_update(p_substitution->left);
			z_update(p_substitution->right);
			z_update(p_substitution);

		} else *node_to_remove = nullptr;

		while (false == stack_path_for_substitution.empty())
			z_update(*stack_path_for_substitution.pop());

		z_balance_stack(stack_for_key);
		dealloc(p_delete_me);
		--size_;
		return true;
	}

	/*
		Nested iterator class
		Supports -> and ++ (TODO: --)
	*/
	struct iterator {
		iterator(ptr_node_t _node = nullptr)
		{ node_ = _node; }

		iterator(const iterator& _i)
		{ node_ = _i.node_; }

		const iterator& operator++() {
			node_ = ff::next_in_binary_tree_with_parent(node_);
			return *this;
		}

		const iterator& operator--() {
			node_ = ff::prev_in_binary_tree_with_parent(node_);
			return *this;
		}

		bool operator!=(const iterator& i) const 
		{ return node_ != i.node_; }

		const ptr_node_t operator->() const 
		{ return node_; }

		friend class amap;

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

	amap(const amap& _amap) 
	{
		/*Not available*/
		this->~amap();
		
		/*for (iterator it = _amap.begin(); it != _amap.end(); ++it) {
			node_t* node = new node_t(it->key, it->val, nullptr);
		}*/
	}

	~amap()
	{ 	
		node_t* r = root_;
		if (nullptr == r)
			return;
		std::stack<ptr_node_t> st;
		st.push(r);
		do {
			node_t* t = st.top(); st.pop();
			if (t->right) st.push(t->right);
			if (t->left)  st.push(t->left );
			dealloc(t);
		} while (!st.empty());
	}

	const amap& operator=(const amap& _amap)
	{/*Not available*/}


	void deep_copy(const amap& that) {
		
	}

protected:

	inline void z_balance_stack(parent_stack_t& parents) {
		ptr_node_t* node = nullptr;
		while (false == parents.empty()) {
			node = parents.pop();
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

	ptr_node_t* z_find_substitution(ptr_node_t& root, parent_stack_t& parents) {
		if (ptr_node_t* pRetVal = getOnlyOneChild(root))
			return pRetVal;

		if (root->left)
			return z_find_very_right_child(root->left, parents);

		if (root->right)
			return z_find_very_left_child(root->right, parents);

		return 0;
	}

	ptr_node_t* getOnlyOneChild(ptr_node_t root) {
		if (root && root->childCount == 1)
			return root->left ?  &(root->left) : &(root->right);
		return 0;
	}

	ptr_node_t* z_find_very_left_child(ptr_node_t& root, parent_stack_t& parents) {
		ptr_node_t* node = &root;
		size_t i = 0;
		while ((*node)->left) {
			parents.push(node);
			node = &((*node)->left);
		}
		parents.push(node);
		return node;
	}

	ptr_node_t* z_find_very_right_child(ptr_node_t& root, parent_stack_t& parents) {
		ptr_node_t* node = &root;
		size_t i = 0;
		while ((*node)->right) {
			parents.push(node);
			node = &((*node)->right);
		}
		parents.push(node);
		return node;
	}

	ptr_node_t* z_find_node_ptr_ptr(ptr_node_t* root, const key_t& key, parent_stack_t& parents) {
		ptr_node_t* node = root;
		size_t i = 0;
		while(*node && (*node)->first != key) {
			parents.push(node);
			node = z_p_child_on_predicate(*node, key);
		}
		return node;
	}

	ptr_node_t* z_find_node_ptr_ptr(ptr_node_t* root, const key_t& key) {
		ptr_node_t* node = root;
		size_t i = 0;
		while(*node && (*node)->first != key)
			node = z_p_child_on_predicate(*node, key);
		return node;
	}

	const value_t* z_find_node(const ptr_node_t root, const key_t& key) const {
		ptr_node_t current = root;

		while (current && current->first != key)
			current = z_child_on_predicate(current, key);
		
		return current ? &(current->second) : 0;
	}

	inline const ptr_node_t& z_child_on_predicate(const ptr_node_t root, const key_t& key) const {
		return (predicate_(root->first, key)) ? root->right : root->left;
	}

	inline ptr_node_t* z_p_child_on_predicate(const ptr_node_t root, const key_t& key) {
		return (predicate_(root->first, key)) ? &(root->right) : &(root->left);
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

	// theoretical upper bound for AVL tree height + 2
	inline size_t bound_height() {
		const double fi = 1.618; // golden ratio
		const double nom = sqrt(5)*(size_+2);
		return (size_t)ceil(log(nom)/log(fi));
	}

	void dealloc(ptr_node_t node) {
		if(node)
			allocator_.deallocate(node, 1);
		node = 0;
	}

	inline ptr_node_t alloc(const key_t& key, const value_t& val, ptr_node_t parent) {
		ptr_node_t ptr = allocator_.allocate(1,0);
		allocator_.construct(ptr, node_t(key, val, parent));
		return ptr;
	}

	ptr_node_t root_;
	pred_t	 predicate_;
	alloc_t	 allocator_;
	size_t   size_;
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
		if (root != 0) {
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
	{ return checkParents_internal(root_, 0); }
	// returns true if childCount property is set correctly in every node
	bool check_children() 
	{ return check_child_counts(root_); }

#endif//_DEBUG
};
} // namespace ff
#endif//__FF_AVLTREE__
