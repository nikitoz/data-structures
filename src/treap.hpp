#ifndef __FF__TREAP__
#define __FF__TREAP__
#include <functional>
#include "base_iterator.hpp"

/*
	Simple treap implementation
	Mostly recursive
*/
class treap_test;
namespace ff {
template
<
	  typename TTree
	, typename THeap
	, typename TTreePred = std::less<TTree>
	, typename THeapPred = std::less<THeap>
>
class treap
{
protected:
	typedef TTree      key_t;
	typedef THeap      heap_t;
	typedef TTreePred  tree_pred_t;
	typedef THeapPred  theap_pred_t;

	friend class treap_test;

	/*
		Treap node type definition
	*/

	struct node
	{
		typedef std::pair<key_t, heap_t> data_t;
		// first - value_key, second - heap_key
		data_t d_;
		node*  left_;
		node*  right_;

		node(const key_t& td, const heap_t& hd, node* left = 0, node* right = 0)
			: d_(td, hd)
			, left_ (left)
			, right_(right)
		{ }
	}; // node<KeyValue, HeapValue>

	typedef node    node_t;

public:
	struct iterator : public ff::base_iterator<node_t, typename node_t::data_t>
	{
		iterator(node_t*& node)
			: node_(node) { }

		inline data_t& operator*() {
			return node_->d_;
		}

		inline data_t* operator->() {
			return &node_->d_;
		}

		inline bool operator==(const iterator& that) const {
			return this->node_ == that.node_;
		}

		inline bool operator!=(const iterator& that) const {
			return this->node_ != that.node_;
		}

		inline iterator &operator++() {
			node_ = ff::next_in_binary_tree_with_parent(node_);
			return *this;
		}
		inline iterator operator++(int) {
			iterator r = *this;
			node_ = ff::next_in_binary_tree_with_parent(node_);
			return r;
		}
		inline iterator &operator--() {
			node_ = ff::prev_in_binary_tree_with_parent(node_);
			return *this;
		}
		inline iterator operator--(int) {
			iterator r = *this;
			node_ = ff::prev_in_binary_tree_with_parent(node_);
			return r;
		}
		friend class treap;

	protected:
		node_t*& node_;
	}; // iterator

	static node_t* shared_null;

public:
	treap()
		: root_ (0)
		, count_(0)
	{ }

	~treap() {
		clear_r();
	}

	void insert_r(const key_t& td, const heap_t& hd) {
		root_ = z_insert_r(root_, td, hd);
	}

	void erase_r(const key_t& td) {
		root_ = z_erase_r(root_, td);
	}

	const key_t& top() const {
		return root_->d_.first;
	}

	bool contains_t(const key_t& td) const {
		return z_find_r(root_, td) != 0;
	}

	size_t count() const {
		return count_;
	}

	void clear_r() {
		z_clear_r(root_);
		root_ = 0;
	}

	iterator find_r(const key_t& key) {
		if (node_t*& node = z_find_r(root_, key))
			return iterator(node);
		return end();
	}

	bool empty() const {
		return 0 == root_;
	}

	void heapify_r(iterator it) {
		z_heapify_r(it.node_);
	}

	void heapify_r() {
		z_heapify_r(root_);
	}

	iterator begin() const {
		return 
	}

	iterator end() const {
		return iterator(shared_null);
	}

	// TODO: rewrite me
	iterator cend() const {
		return iterator(shared_null);
	}

protected:
	node_t*   root_ ;
	size_t    count_;
	TTreePred tree_pred_;
	THeapPred heap_pred_;

private:
	node_t* z_erase_r(node_t* r, const key_t& td) {
		if (0 == r)
			return 0;
		if (td == r->d_.first) {
			node_t* ret = nullptr;
			if (0 == r->left_)
				ret = r->right_;
			else if (0 == r->right_)
				ret = r->left_;
			else {
				ret = z_disconnect_smallest(r->right_);
				ret->left_  = r->left_;
				ret->right_ = r->right_;
				z_heapify_r(ret);
			}
			--count_;
			delete r;
			r = ret;
		} else if (tree_pred_(td, r->d_.first)) {
			r->left_  = z_erase_r(r->left_ , td);
		} else {
			r->right_ = z_erase_r(r->right_, td);
		}
		return r;
	}

	node_t*& z_find_r(node_t* r, const TTree& td) const {
		if (0 == r)
			return shared_null;
		if (r->d_.first == td)
			return r;
		return z_find_r(tree_pred_(td, r->d_.first) ? r->left_ : r->right_ , td);
	}

	node_t* z_disconnect_smallest(node_t*& root) {
		if (root->left_ == 0) {
			node_t* tmp = root;
			root = tmp->right_;
			return tmp;
		}
		node_t* r = root;
		while (r->left_->left_)
			r = r->left_;
		node_t* ret = r->left_;
		r->left_ = ret->right_;
		return ret;
	}

	void z_heapify_r(node_t*& r) {
		if (0 == r) return;
		if ( r->right_ && !heap_pred_(r->d_.second, r->right_->d_.second) ) {
			z_rotate_left(r);
			z_heapify_r(r->left_);
		}
		if ( r->left_  && !heap_pred_(r->d_.second, r->left_ ->d_.second) ) {
			z_rotate_right(r);
			z_heapify_r(r->right_);
		}
	}

	node_t* z_insert_r(node_t* root, const key_t& td, const heap_t& hd) {
		if (0 == root) {
			++count_;
			return new node_t(td, hd);
		}

		if (root->d_.second == hd || root->d_.first == td)
			return root;

		if (tree_pred_(td, root->d_.first))
			root->left_  = z_insert_r(root->left_ , td, hd);
		else
			root->right_ = z_insert_r(root->right_, td, hd);

		z_heapify_r(root);
		return root;
	}

	void z_clear_r(node_t* r) {
		if (0 == r) return;
		z_clear_r(r->right_);
		z_clear_r(r->left_);
		delete r;
	}

	//a      b
	// \ => /
	//  b  a
	void z_rotate_left(node_t*& a) {
		node_t* b  = a->right_;
		node_t* bl = b->left_;
		node_t* ta = a;
		a = b;
		b->left_   = ta;
		ta->right_ = bl;
	}

	//  a  b
	// / => \
	//b      a
	void z_rotate_right(node_t*& a) {
		node_t* b  = a->left_ ;
		node_t* br = b->right_;
		node_t* ta = a;
		a = b;
		b->right_ = ta;
		ta->left_ = br;
	}
};     // treap

/*
	Init shared null link with null
*/
template
<
	  typename TTree
	, typename THeap
	, typename TTreePred
	, typename THeapPred
>
typename treap<TTree, THeap, TTreePred, THeapPred>::node_t* treap<TTree, THeap, TTreePred, THeapPred>::shared_null = nullptr;
}      // ff
#endif // __FF_TREAP__