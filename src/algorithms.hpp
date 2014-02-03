#ifndef __FF__ALGORITHMS__
#define __FF__ALGORITHMS__
namespace ff {
template <typename T>
T*& next_in_binary_tree_with_parent(T* node) {
	ptr_node_t prev = node;
	if (node->right) {
		node = node->right;
		while(node->left) 
			node = node->left;
		return node;
	}

	node_ = node->parent;
	if (!node)
		return 0;// end()

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
} // ff
#endif // __FF__ALGORITHMS__
