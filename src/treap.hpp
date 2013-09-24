#ifndef __FF__TREAP__
#define __FF__TREAP__
#include <functional>

/*
	Set implementation, using treap
	Mostly recursive
*/

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
public:
	treap()
		: root_ (0)
		, count_(0)
	{ }

	~treap() {
		clear_r();
	}

	void insert_r(const TTree& td, const THeap& hd) {
		root_ = z_insert_r(root_, td, hd);
	}

	void erase_r(const TTree& td) {
		root_ = z_erase_r(root_, td);
	}

	bool contains_t(const TTree& td) const {
		return z_find_r(root_, td) != 0;
	}

	size_t count() const {
		return count_;
	}

	void clear_r() {
		z_clear_r(root_);
		root_ = 0;
	}

private:
	typedef node_t* link;

	link z_erase_r(link r, const TTree& td) {
		if (0 == r)
			return 0;
		if (td == r->td_) {
			link ret = 0;
			if (0 == r->left_)
				ret = r->right_;
			else if (0 == r->right_)
				ret = r->left_;
			else {
				ret = z_disconnect_smallest(r->right_);
				ret->left_  = r->left_;
				ret->right_ = r->right_;
				heapify(ret);
			}
			--count_;
			delete r;
			r = ret;
		} else if (tree_pred_(td, r->td_)) {
			r->left_  = z_erase_r(r->left_ , td);
		} else {
			r->right_ = z_erase_r(r->right_, td);
		}
		return r;
	}

	link z_find_r(link r, const TTree& td) const {
		if (0 == r)
			return 0;
		if (r->td_ == td)
			return r;
		if (tree_pred_(td, r->td_))
			return z_find_r(r->left_ , td);
		else
			return z_find_r(r->right_, td);
	}

	link z_disconnect_smallest(link& root) {
		if (root->left_ == 0) {
			link tmp = root;
			root = tmp->right_;
			return tmp;
		}
		link r = root;
		while (r->left_->left_)
			r = r->left_;
		link ret = r->left_;
		r->left_ = ret->right_;
		return ret;
	}

	void heapify(link& r) {
		if (0 == r) return;
		if ( r->right_ && !heap_pred_(r->hd_, r->right_->hd_) ) {
			z_rotate_left(r);
			heapify(r->left_);
		}
		if ( r->left_  && !heap_pred_(r->hd_, r->left_ ->hd_) ) {
			z_rotate_right(r);
			heapify(r->right_);
		}
	}

	link z_insert_r(link root, const TTree& td, const THeap& hd) {
		if (0 == root) {
			++count_;
			return new node_t(td, hd);
		}

		if (root->hd_ == hd || root->td_ == td)
			return root;

		if (tree_pred_(td, root->td_))
			root->left_  = z_insert_r(root->left_ , td, hd);
		else
			root->right_ = z_insert_r(root->right_, td, hd);

		if ( root->right_ && !heap_pred_(root->hd_, root->right_->hd_) )
			z_rotate_left(root);
		if ( root->left_  && !heap_pred_(root->hd_, root->left_ ->hd_) )
			z_rotate_right(root);

		return root;
	}

	void z_clear_r(link r) {
		if (0 == r) return;
		z_clear_r(r->right_);
		z_clear_r(r->left_);
		delete r;
	}

	//a      b
	// \ => /
	//  b  a
	void z_rotate_left(link& a) {
		link b  = a->right_;
		link bl = b->left_;
		link ta = a;
		a = b;
		b->left_   = ta;
		ta->right_ = bl;
	}

	//  a  b
	// / => \
	//b      a
	void z_rotate_right(link& a) {
		//a->left, b->right
		link b  = a->left_ ;
		link br = b->right_;
		link ta = a;
		a = b;
		b->right_ = ta;
		ta->left_ = br;
	}

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
		TT td_;
		TH hd_;
		node<TT, TH>* left_;
		node<TT, TH>* right_;

		node(const TTree& td, const THeap& hd, node* left = 0, node* right = 0)
			: td_(td)
			, hd_(hd)
			, left_ (left)
			, right_(right)
		{ }
	};
	typedef node<TTree, THeap>  node_t;

	node_t* root_ ;
	size_t  count_;
	TTreePred tree_pred_;
	THeapPred heap_pred_;

	/*
		Test stuff
	*/
#ifdef _DEBUG
public:
	bool test_sanity() {
		size_t count = 0;
		bool b = test_sanity_r(root_, count);
		return b && (count == count_);
	}

	bool test_sanity_r(const link r, size_t& count) {
		if (0 == r) return true;
		++count;
		return (r->left_  == 0 || (tree_pred_(r->left_->td_, r->td_)  && heap_pred_( r->hd_, r->left_->hd_)) )
			&& (r->right_ == 0 || (!tree_pred_(r->right_->td_, r->td_) && heap_pred_( r->hd_,r->right_->hd_)) )
			&& test_sanity_r(r->right_, count) && test_sanity_r(r->left_, count);
	}
	#include "fftest.hpp"
	//a      b
	// \ => /
	//  b  a
	static void test_rotate_left() {
		test::say("Left rotate test:");
		node_t* a = new node_t('a', 'a');
		node_t* b = new node_t('b', 'b');
		node_t* c = new node_t('c', 'c');
		node_t* d = new node_t('d', 'd');
		node_t* e = new node_t('e', 'e');
		a->right_ = b;
		a->left_  = c;
		b->left_  = d;
		b->right_ = e;
		ff::treap<int, int> t;
		t.z_rotate_left(a);
		test::verify(a->hd_ == 'b'               , "b == a");
		test::verify(a->left_->hd_ == 'a'        , "b->left_ == a");
		test::verify(a->left_->left_->hd_ == 'c' , "b->left_->left_ == c");
		test::verify(a->left_->right_->hd_ == 'd', "b->left_->right_ == d");
		test::verify(a->right_->hd_ == 'e'       , "b->right_ == e");
	}
	//  a  b
	// / => \
	//b      a
	static void test_rotate_right() {
		test::say("Right rotate test:");
		node_t* a = new node_t('a', 'a');
		node_t* b = new node_t('b', 'b');
		node_t* c = new node_t('c', 'c');
		node_t* d = new node_t('d', 'd');
		node_t* e = new node_t('e', 'e');
		a->left_  = b;
		a->right_ = c;
		b->left_  = d;
		b->right_ = e;
		ff::treap<int, int> t;
		t.z_rotate_right(a);
		test::verify(a->hd_ == 'b'                , "b == a");
		test::verify(a->right_->hd_ == 'a'        , "b->right_ == a");
		test::verify(a->left_->hd_ == 'd'         , "b->left_ == d");
		test::verify(a->right_->left_->hd_ == 'e' , "b->right_->left_ == e");
		test::verify(a->right_->right_->hd_ == 'c', "b->right_->right_ == c");
	}

	#include <cstdio>
	void print(const char* path) const {
		FILE* f = fopen(path, "wt");
		fprintf(f, "DIGRAPH g {\n");
		print_r(root_, f);
		fprintf(f, "};");
		fclose(f);
	}

	void print(const TTree& td, const char* path) const {
		link r = z_find_r(root_, td);
		FILE* f = fopen(path, "wt");
		fprintf(f, "DIGRAPH g {\n");
		print_r(r, f);
		fprintf(f, "};");
		fclose(f);
	}

	void print_r(const link r, FILE*f) const {
		if (0 == r || 0 == f) return;
		if (0 != r->right_) {
			fprintf(f, "\"%d\|%d\"->\"%d\|%d\"\n", r->td_, r->hd_, r->right_->td_, r->right_->hd_);
			print_r(r->right_, f);
		}
		if (0 != r->left_) {
			fprintf(f, "\"%d\|%d\"->\"%d\|%d\"\n", r->td_, r->hd_, r->left_->td_, r->left_->hd_);
			print_r(r->left_, f);
		}
	}
#endif //_DEBUG /* End test stuff*/
};     // treap
}      // ff
#endif // __FF_TRIE__