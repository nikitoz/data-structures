#ifndef __FF_BST__
#define __FF_BST__
#include <iostream>
namespace ff {
namespace {
template <typename T>
struct node {
	node* left_;
	node* right_;
	T     data_;
	node(const T& data, node* left = 0, node* right = 0) 
		: data_(data), left_(left), right_(right) { }
};
}// anonymous-namespace

template <typename T, typename TPred>
struct bst {
	typedef node <T> node_t;
	node_t* root;
	TPred   pred;// left - true, right - false

	bst() 
		: root(0) { }

	~bst() {
		cleanup();
	}

	void insert(const T& data) {
		if (root == 0) return (void)(root = new node_t(data));
		node_t* t = root;
		while (t && t->data_ != data) {
			bool condition = pred (data, t->data_);
			if (condition && t->left_ == 0)
				return (void)(t->left_ = new node_t(data));
			else if (!condition && t->right_ == 0)
				return (void)(t->right_ = new node_t(data));
			else
				t = condition ? t->left_ : t->right_;
		}
	}

	node_t* search(const T& data) {
		node_t* t = root;
		while (t && t->data_ != data) {
			t = pred(data, t->data_) ? t->left_ : t->right_;
		}
		return t;
	}

	void remove(const T& data) {
		node_t* p = root;
		node_t* t = root;
		while (t && t->data_ != data) {
			p = t;
			t = pred(data, t->data_) ? t->left_ : t->right_;
		}

		if (0 == t) return;
		if (0 == t->right_) {
			if (t == p->left_)
				p->left_ = t->left_;
			else if (t == p->right_)
				p->right_ = t->left_;
			else if (t == p)
				root = t->left_;
		} else if (0 == p->left_) {
			if (t == p->left_)
				p->left_ = t->right_;
			else if (t == p->right_)
				p->right_ = t->right_;
			else if (t == p)
				root = t->right_;
		} else {
			node_t* substitution = t->right_;
			node_t* substitution_parent = t;
			while (substitution->left_) {
				substitution_parent = substitution;
				substitution = substitution->left_;
			}

			if (substitution_parent != t) {
				if (substitution_parent->left_ == substitution)
					substitution_parent->left_ = substitution->right_;
				else
					substitution_parent->right_ = substitution->right_;
			}

			if (t == p->left_)
				p->left_ = substitution;
			else if (t == p->right_)
				p->right_ = substitution;
			else
				root = substitution;
			substitution->left_ =  t->left_;
			if (substitution != t->right_)
				substitution->right_ = t->right_;
		}
		delete t;
	}

	node_t* least(node_t* r) {
		if (0 == r)
			return 0;
		while (r->left_)
			r = r->left_;
		return r;
	}

	void erase(const T& data) {
		root = erase_ir(root, data);
	}

	void from_sorted(const T* a,  int n) {
		cleanup();
		root = from_sorted_i(a, 0, n);
	}

	void print_inorder_r() {
		print_inorder_ir(root);
		std::cout << std::endl;
	}

	void print_inorder() {
		node_t* r = root;
		std::stack<node_t*> st;
		do { st.push(r); r = r->left_; } while (r);
		
		do {
			r = st.top(); st.pop();
			std::cout << r->data_ << " ";
			if (0 == r->right_) continue;
			r = r->right_;
			st.push(r);
			while ((r = r->left_)) { st.push(r); };
		} while (!st.empty());
		std::cout << std::endl;
	}

	void print_preorder() {
		node_t* r = root;
		if (0 == r)
			return;
		std::stack<node_t*> st;
		st.push(r);

		do {
			node_t* t = st.top(); st.pop();
			std::cout << t->data_ << " ";
			if (t->right_) st.push(t->right_);
			if (t->left_)  st.push(t->left_ );
		} while (!st.empty());
		std::cout << std::endl;
	}

	void print_postorder() {
		node_t* r = root;
		if (0 == r)
			return;
		
		node_t* prev = r;
		std::stack<node_t*> st;
		while (r) { st.push(r); prev = r; r = r->left_; }
		
		do {
			r = st.top();
			if (prev == r->right_) {
				st.pop();
				std::cout << r->data_ << ' ';
				prev = r; 
			} else {
				if (0 == r->right_) {
					std::cout << r->data_ << ' ';
					st.pop();
					prev = r;
				}
				r = r->right_;
				while (r) { st.push(r); prev = r; r = r->left_; }
			}
		} while (!st.empty());
		std::cout << std::endl;
	}

	void print_preorder_r() {
		print_preorder_ir(root);
		std::cout << std::endl;
	}

	void print_postorder_r() {
		print_postorder_ir(root);
		std::cout << std::endl;
	}

	void cleanup() {
		cleanup_i(root);
	}

	node_t* dll() {
		return dll_ir(root, false);
	}

private:
	void print_postorder_ir(node_t* r) {
		if (0 == r)
			return;
		print_postorder_ir(r->left_ );
		print_postorder_ir(r->right_);
		std::cout << r->data_ << ' ';
	}

	void print_preorder_ir(node_t* r) {
		if (0 == r)
			return;
		std::cout << r->data_ << " ";
		print_preorder_ir(r->left_);
		print_preorder_ir(r->right_);
	}

	node_t* dll_ir(node_t* r, bool is_left) {
		if (0 == r)
			return 0;
		node_t* left  = dll_ir(r->left_, true);
		node_t* t = new node_t(r->data_, left);
		if (left)
			left->right_ = t;
		node_t* right = dll_ir(r->right_, false);
		t->right_ = right;
		if (right)
			right->left_ = t;

		if (!is_left)
			while (t->left_)  t = t->left_;
		else
			while (t->right_) t = t->right_;

		return t;
	}

	node_t* erase_ir(node_t* r, const T& data) {
		if (0 == r)
			return 0;
		if (r->data_ == data) {
			node_t* ret = 0;
			if (0 == r->left_)
				ret = r->right_;
			else if (0 == r->right_)
				ret = r->left_;
			else {
				node_t* substitution = least(r->right_);
				ret = new node_t(substitution->data_);
				r->right_ = erase_ir(r->right_, substitution->data_);
				ret->left_ = r->left_;
				ret->right_= r->right_;
			}
			delete r;
			return ret;
		} else if (pred(data, r->data_)) {
			r->left_  = erase_ir(r->left_, data);
		} else {
			r->right_ = erase_ir(r->right_, data);
		}
		return r;
	}

	node_t* from_sorted_i(const T* a, int s, int e) {
		if (a == 0 || e <= s) return 0;
		int median = s + ((e-s)/2);
		return new node_t(a[median], from_sorted_i(a, s, median), from_sorted_i(a, median+1, e));
	}

	void print_inorder_ir( node_t* r ) {
		if (0 == r) return;
		print_inorder_ir(r->left_);
		std::cout << r->data_ << " ";
		print_inorder_ir(r->right_);
	}

	void cleanup_i(node_t* r) {
		if (0 == r) return;
		cleanup_i(r->left_);
		cleanup_i(r->right_);
		delete r;
	}
};// bst
} // ff
#endif // __FF_BST__