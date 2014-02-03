#ifndef __FF__TREAP__
#define __FF__TREAP__
#include <functional>
#include "base_iterator.hpp"

/*
	Treap implementation
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
	
	/*
		Data
	*/

	template 
	<
		  typename TT  = TTree
		, typename TH  = THeap
	>
	struct node
	{
		typedef std::pair<TT, TH> data_t;
		// first - value_key, second - heap_key
		std::pair<TT, TH> d_;
		node<TT, TH>*     left_;
		node<TT, TH>*     right_;

		node(const TTree& td, const THeap& hd, node* left = 0, node* right = 0)
			: d_(td, hd)
			, left_ (left)
			, right_(right)
		{ }
	}; // node<KeyValue, HeapValue>

	typedef node<TTree, THeap>  node_t;
	typedef node_t*             link_t;

public:
	friend class treap_test;
	typedef TTree key_value_t;
	typedef THeap heap_value_t;

	struct iterator : public ff::base_iterator<node_t, typename node_t::data_t>
	{
		iterator(link_t& node) : node_(node) { }

		data_t& operator*() {
			return node_.d_;
		}

		data_t* operator->() {
			return &node_->d_;
		}

		bool operator==(const iterator& that) const {
			return this->node_ == that.node_;
		}

		bool operator!=(const iterator& that) const {
			return this->node_ != that.node_;
		}
		friend class treap;
	protected:
		link_t& node_;
	}; // iterator

	static link_t shared_null;

public:
	treap()
		: root_ (0)
		, count_(0)
	{ }

	~treap() {
		clear_r();
	}

	void insert_r(const key_value_t& td, const heap_value_t& hd) {
		root_ = z_insert_r(root_, td, hd);
	}

	void erase_r(const key_value_t& td) {
		root_ = z_erase_r(root_, td);
	}

	const key_value_t& top() const {
		return root_->d_.first;
	}

	bool contains_t(const key_value_t& td) const {
		return z_find_r(root_, td) != 0;
	}

	size_t count() const {
		return count_;
	}

	void clear_r() {
		z_clear_r(root_);
		root_ = 0;
	}

	iterator find_r(const key_value_t& key) {
		if (link_t& node = z_find_r(root_, key))
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

	iterator end() const {
		return iterator(shared_null);
	}

	// TODO: rewrite me
	iterator cend() const {
		return iterator(shared_null);
	}

protected:
	link_t    root_ ;
	size_t    count_;
	TTreePred tree_pred_;
	THeapPred heap_pred_;

private:
	link_t z_erase_r(link_t r, const key_value_t& td) {
		if (0 == r)
			return 0;
		if (td == r->d_.first) {
			link_t ret = 0;
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

	link_t& z_find_r(link_t& r, const TTree& td) const {
		if (0 == r)
			return shared_null;
		if (r->d_.first == td)
			return r;
		return z_find_r(tree_pred_(td, r->d_.first) ? r->left_ : r->right_ , td);
	}

	link_t z_disconnect_smallest(link_t& root) {
		if (root->left_ == 0) {
			link_t tmp = root;
			root = tmp->right_;
			return tmp;
		}
		link_t r = root;
		while (r->left_->left_)
			r = r->left_;
		link_t ret = r->left_;
		r->left_ = ret->right_;
		return ret;
	}

	void z_heapify_r(link_t& r) {
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

	link_t z_insert_r(link_t root, const key_value_t& td, const heap_value_t& hd) {
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

	void z_clear_r(link_t r) {
		if (0 == r) return;
		z_clear_r(r->right_);
		z_clear_r(r->left_);
		delete r;
	}

	//a      b
	// \ => /
	//  b  a
	void z_rotate_left(link_t& a) {
		link_t b  = a->right_;
		link_t bl = b->left_;
		link_t ta = a;
		a = b;
		b->left_   = ta;
		ta->right_ = bl;
	}

	//  a  b
	// / => \
	//b      a
	void z_rotate_right(link_t& a) {
		//a->left, b->right
		link_t b  = a->left_ ;
		link_t br = b->right_;
		link_t ta = a;
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
typename treap<TTree, THeap, TTreePred, THeapPred>::link_t treap<TTree, THeap, TTreePred, THeapPred>::shared_null = 0;
}      // ff
#endif // __FF_TREAP__