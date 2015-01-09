#ifndef __FF__ITERATOR__ALGORITHMS__
#define __FF__ITERATOR__ALGORITHMS__
namespace ff {

bool fn_true()  { return true;  }
bool fn_false() { return false; }

template <typename T>
T* next_in_binary_tree_with_parent(T* node) {
	T* prev = node;
	if (node->right_) {
		node = node->right_;
		while(node->left_) 
			node = node->left_;
		return node;
	}

	node = node->parent_;
	if (!node)
		return nullptr;// end()

	// if prev was left then visit this node
	if (node->left_ == prev)
		return node; // then go to right

	// if prev was right then go up, until we're not in right subtree
	while(node && node->right_ == prev) {
		prev = node;
		node = node->parent_;
	}
	return node;
} // next_in_binary_tree_with_parent

template <typename T>
T* prev_in_binary_tree_with_parent(T* node) {
	T* prev = node;
	if (node->left_) {
		node = node->left_;
		while(node->right_) 
			node = node->right_;
		return node;
	}

	node = node->parent_;
	if (!node)
		return nullptr;

	if (node->right_ == prev)
		return node;

	while(node && node->left_ == prev) {
		prev = node;
		node = node->parent_;
	}
	return node;
} // prev_in_binary_tree_with_parent
} // ff
#endif // __FF__ITERATOR__ALGORITHMS__
