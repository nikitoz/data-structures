#ifndef __FF__TREAP__TEST__
#define __FF__TREAP__TEST__
#include "../src/treap.hpp"
#include "fftest.hpp"

class treap_test {
	typedef ff::treap<int, int>   treap_t;
	typedef treap_t::node_t       node_t;
	typedef treap_t::key_t  key_value_t;
	typedef treap_t::heap_t heap_value_t;
	typedef treap_t::node_t*       link_t;
public:
	treap_t tr_;

	bool test_sanity() {
		size_t count = 0;
		bool b = test_sanity_r(tr_.root_, count);
		return b && (count == tr_.count_);
	}

	bool test_sanity_r(const link_t r, size_t& count) {
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
		using namespace ff;
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
		using namespace ff;
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

	void print(const key_value_t& td, const char* path) const {
		link_t r = z_find_r(tr_.root_, td);
		FILE* f = fopen(path, "wt");
		fprintf(f, "DIGRAPH g {\n");
		print_r(r, f);
		fprintf(f, "};");
		fclose(f);
	}

	void print_r(const link_t r, FILE*f) const {
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
};     // class treap_test
#endif // __FF__TREAP__TEST__