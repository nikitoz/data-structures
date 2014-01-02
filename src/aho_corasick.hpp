#ifndef __FF__AHO__CORASICK__
#define __FF__AHO__CORASICK__
#include "trie.hpp"
#include <exception>
#include <stack>
#include <memory>
#include <stdio.h>
#include <cctype>
#include <cstring>
#include <map>
namespace ff {

	class aho_corasick : public ff::trie< ff::aho_tnode<> > {
	typedef ff::trie< aho_tnode<> > parent_t;

public:
	aho_corasick()
		: parent_t()
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
	TContainer match(tchar_t* text) const {
		TContainer ret_cont;


		//... TODO

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
			node_t* sf = c->suffix_->next_[i];
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
				c->next_[i] = new node_t(c, tolower(*s), 0);
			}
			c = c->next_[i];
		}
		c->d_       = data;
		c->is_leaf_ = true;
		return c;
	}
public:
#if defined(_DEBUG) || !defined(NDEBUG)
	bool test_count() {
		return test_count_r(root_);
	}

	bool test_count_r(node_t* root) {
		if (0 == root) return true;
		int sum = 0;
		for (int i = 0; i != NUM_LETTERS; ++i)
			if (0 != root->next_[i]) ++sum;

		if (sum != root->child_count_) return false;

		bool res = true;
		for (int i = 0; i != NUM_LETTERS; ++i)
			if (0 != root->next_[i]) res &= test_count_r(root->next_[i]);
		return res;
	}

	void print() const {
		std::map<const node_t*, int> maps;
		FILE* f = fopen("D:\\tex.txt", "wt");
		fprintf(f, "DIGRAPH g {\n");
		fill_cache_map_r(root_, 0, maps);
		print_r(root_, 0, f, maps);
		fprintf(f, "};");
		fclose(f);
	}

	void fill_cache_map_r(const node_t* a, int start, std::map<const node_t*, int>& maps) const {
		maps[a] = start;
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next_[chrToIdx(c)]) {
				maps[a->next_[chrToIdx(c)]] = start + 1 + chrToIdx(c);
				fill_cache_map_r(a->next_[chrToIdx(c)], start + 1 + chrToIdx(c), maps);
			}
	}

	void print_r(const node_t* a, int start, FILE* f, std::map<const node_t*, int>& maps) const {
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next_[chrToIdx(c)]) {
				fprintf(f, "%c%d->%c%d;\n", a->ch_, start, c, start + 1 + chrToIdx(c));
				print_r(a->next_[chrToIdx(c)], start + 1 + chrToIdx(c), f, maps);
			}

		fprintf(f, "%c%d->%c%d [color=\"red\"];\n", a->ch_, start, a->suffix_->ch_, maps[a->suffix_]);
	}
#endif // _DEBUG

}; // aho_corasick
}; // ff

#endif // __FF__AHO__CORASICK__
