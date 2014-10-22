#ifndef __FF__ALGORITHMS__
#define __FF__ALGORITHMS__
namespace ff {

bool fn_true()  { return true;  }
bool fn_false() { return false; }

template <typename T>
T* next_in_binary_tree_with_parent(T* node) {
	T* prev = node;
	if (node->right) {
		node = node->right;
		while(node->left) 
			node = node->left;
		return node;
	}

	node = node->parent;
	if (!node)
		return nullptr;// end()

	// if prev was left then visit this node
	if (node->left == prev)
		return node; // then go to right

	// if prev was right then go up, until we're not in right subtree
	while(node && node->right == prev) {
		prev = node;
		node = node->parent;
	}
	return node;
} // next_in_binary_tree_with_parent

template <typename T>
T* prev_in_binary_tree_with_parent(T* node) {
	T* prev = node;
	if (node->left) {
		node = node->left;
		while(node->right) 
			node = node->right;
		return node;
	}

	node = node->parent;
	if (!node)
		return nullptr;

	if (node->right == prev)
		return node;

	while(node && node->left == prev) {
		prev = node;
		node = node->parent;
	}
	return node;
} // prev_in_binary_tree_with_parent

template <typename R, typename O, typename P>
struct bind1st {
	typedef R (O::*F)(P);

	bind1st(F* fn, O* o) : o_(o), fn_(fn) { }
	R operator()(const P& p) {
		return o->(*fn)(p);
	}
private:
	O* o_;
	F* fn_;
};
} // ff
#endif // __FF__ALGORITHMS__
