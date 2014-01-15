#ifndef __FF__AHO__CORASICK__
#define __FF__AHO__CORASICK__
#include "trie.hpp"
#include <exception>
#include <stack>
#include <memory>
#include <cctype>
#include <cstring>

class aho_printer;

namespace ff {

class aho_corasick : public trie< aho_tnode<> > {
	typedef aho_tnode<> node_t;
public:
	aho_corasick()
		: trie()
	{ }

	template<typename TIter, typename TCons>
	void build_from(TIter begin, TIter end, TCons cons) {
		this->~aho_corasick();
		new (this) aho_corasick;
		root_->suffix_ = root_;

		TIter b = begin;
		for (int i = 0; begin != end; ++begin, ++i)
			insert(cons(*begin), i);

		for (; b != end; ++b)
			z_make_fail_links(root_, cons(*b));
	}

	void insert(const tchar_t* str, const tdata_t& data) {
		z_insert(root_, str, data);
	}

	template <typename TContainer>
	TContainer match(const tchar_t* text) const {
		TContainer ret_cont;
		int i;
		node_t* r = root_;
		for (const tchar_t* s = text; *s;) {
			i = chrToIdx(tolower(*s));
			if (r->next_[i]) {
				r = r->next_[i];
				if (r->is_leaf_) {
					ret_cont.insert(ret_cont.end(), r->d_);
					node_t* p = r;
					while ((p = p->suffix_) && p->is_leaf_)
						ret_cont.insert(ret_cont.end(), p->d_);
				}
				++s;
			} else if (r == root_) {
				++s;
			} else {
				r = r->suffix_;
			}
		}

		return ret_cont;
	}

protected:
	void z_make_fail_links(node_t* root, const tchar_t* str) {
		if (0 == str || 0 == root) return;
		// set fail link of the first letter
		int i = chrToIdx(tolower(*str));
		root->next_[i]->suffix_ = root;
		node_t* c  = root->next_[i];

		for (const tchar_t* s = str+1; *s; ++s) {
			i = chrToIdx(tolower(*s));
			node_t* sf = (node_t*)c->suffix_->next_[i];
			if (0 == sf)
				sf = root;
			c->next_[i]->suffix_ = sf;
			c = c->next_[i];
		}
	}

	node_t* z_insert(node_t* root, const tchar_t* str, const tdata_t& data) {
		if (0 == str || 0 == root)
			return 0;
		node_t* c  = root;
		for (const tchar_t* s = str; *s; ++s) {
			const int i = chrToIdx(tolower(*s));
			if (!c->next_[i]) {
				++c->child_count_;
				c->next_[i] = new node_t(tolower(*s), 0);
			}
			c = c->next_[i];
		}
		c->d_       = data;
		c->is_leaf_ = true;
		return c;
	}
	friend class aho_printer;
}; // aho_corasick
}; // ff

#endif // __FF__AHO__CORASICK__
