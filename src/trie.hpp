#ifndef __FF__TRIE__
#define __FF_TRIE__
#include <exception>
#include <stack>
#include <memory>
#include <cstring>
#if defined(_DEBUG) || !defined(NDEBUG)
#include <stdio.h>
#endif // DEBUG
namespace ff {

//namespace { 
template
<
	  typename _TypeChar = char
	, typename _TypeData = int
	, int _FIRST_LETTER = 'a'
	, int _NUM_LETTERS =  'z'-'a' + 1
>
struct tnode_base
{
	enum {
		FIRST_LETTER = _FIRST_LETTER,
		NUM_LETTERS  = _NUM_LETTERS
	};

	typedef _TypeChar TypeChar;
	typedef _TypeData TypeData;

	TypeChar  ch_;
	TypeData  d_;
	char      child_count_;
	bool      is_leaf_;

	tnode_base (TypeChar c = TypeChar(), TypeData d = TypeData())
		: ch_(c)
		, d_ (d)
		, child_count_(0)
		, is_leaf_(false)
	{ }
}; // node

template
<
	  typename _TypeChar = char
	, typename _TypeData = int
	, int _FIRST_LETTER = 'a'
	, int _NUM_LETTERS =  'z'-'a' + 1
>
struct aho_tnode : public ff::tnode_base<_TypeChar, _TypeData, _FIRST_LETTER, _NUM_LETTERS> {
	typedef ff::tnode_base<_TypeChar, _TypeData, _FIRST_LETTER, _NUM_LETTERS> base_t;
	aho_tnode(_TypeChar c = _TypeChar(), _TypeData d = _TypeData())
		: base_t(c, d)
		, suffix_(0)
	{ memset(next_, 0, sizeof(next_)); }

	aho_tnode*   suffix_;
	aho_tnode*   next_[base_t::NUM_LETTERS];
}; // node

template
<
	  typename _TypeChar = char
	, typename _TypeData = int
	, int _FIRST_LETTER = 'a'
	, int _NUM_LETTERS =  'z'-'a' + 1
>
struct tnode : public ff::tnode_base<_TypeChar, _TypeData, _FIRST_LETTER, _NUM_LETTERS> {
	typedef ff::tnode_base<_TypeChar, _TypeData, _FIRST_LETTER, _NUM_LETTERS> base_t;
	tnode(_TypeChar c = _TypeChar(), _TypeData d = _TypeData())
		: base_t(c, d)
	{ memset(next_, 0, sizeof(next_)); }

	tnode*   next_[base_t::NUM_LETTERS];
}; // node
//}  // anonymous-namespace

template
<
typename TNode = ff::tnode<>
>
class trie
{
protected:
	typedef TNode node_t;
	typedef typename node_t::TypeChar tchar_t;
	typedef typename node_t::TypeData tdata_t;
	static const int FIRST_LETTER = node_t::FIRST_LETTER;
	static const int NUM_LETTERS  = node_t::NUM_LETTERS;

	node_t* root_;

public:
	trie()
		: root_(new node_t)
	{ }

	virtual ~trie() {
		z_remove( root_ );
		root_ = 0;
	}

	void insert(const tchar_t* str, const tdata_t& data) {
		z_insert(root_, str, data);
	}

	const tdata_t& try_get(const tchar_t* str) const {
		node_t* n = find(str);
		if (0 == n || false == n->is_leaf_)
 			throw std::exception("value not found");
		return n->d_;
	}

	tdata_t& get(const tchar_t* str) {
		return z_find_insert(root_, str)->d_;
	}

	void remove(const tchar_t* str) {
		if (0 == str)
			return;

		std::stack<node_t*> st;
		node_t* c  = root_;
		for (const tchar_t* s = str; *s; ++s) {
			const int i = chrToIdx(*s);
			if (c->next_[i])
				st.push(c = c->next_[i]);
			else
				return;
		}
		if (false == c->is_leaf_) return;
		c->is_leaf_ = false;
		while (!st.empty()) {
			c = st.top();
			st.pop();
			if (0 == c->child_count_) {
				--c->child_count_;
				const tchar_t t = c->ch_;
				delete c;
				c = st.top();
				c->next_[chrToIdx(tolower(t))] = 0;
			} else break;
		}
	}

protected:

	node_t* z_find(node_t* root, const tchar_t* str) {
		if (0 == str)
			return 0;
		node_t* c  = root_;
		for (const tchar_t* s = str; *s; ++s) {
			const int i = chrToIdx(*s);
			if (c->next_[i])
				c = c->next_[i];
			else
				return 0;
		}
		return c;
	}

	node_t* z_find_insert(node_t* root, const tchar_t* str) {
		if (0 == str || 0 == root)
			return 0;
		node_t* c  = root;
		for (const tchar_t* s = str; *s; ++s) {
			const int i = chrToIdx(*s);
			if (c->next_[i])
				c = (node_t*)c->next_[i];
			else
				return z_insert(c, s, tdata_t());
		}
		c->is_leaf_ = true;
		return c;
	}

	node_t* z_insert(node_t* root, const tchar_t* str, const tdata_t& data) {
		if (0 == str || 0 == root)
			return 0;
		node_t* c  = root;
		for (const tchar_t* s = str; *s; ++s) {
			const int i = chrToIdx(tolower(*s));
			if (!c->next_[i]) {
				++c->child_count_;
				c->next_[i] = new node_t(tolower(*s), data);
			}
			c = c->next_[i];
		}
		c->d_       = data;
		c->is_leaf_ = true;
		return c;
	}

	void z_remove(node_t* r) {
		if (0 == r)
			return;
		for (int i = 0; i != NUM_LETTERS; ++i)
			if (r->next_[i])
				z_remove(r->next_[i]);
		delete r;
	}

	int chrToIdx(tchar_t ch) const
	{ return ch - FIRST_LETTER;}

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
		FILE* f = fopen("D:\\tex.txt", "wt");
		fprintf(f, "DIGRAPH g {\n");
		print_r(root_, 0, f);
		fprintf(f, "};");
		fclose(f);
	}

	void print_r(const node_t* a, int start, FILE* f) const {
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next_[chrToIdx(c)]) {
				fprintf(f, "%c%d->%c%d;\n", a->ch_, start, c, start + 1 + chrToIdx(c));
				print_r(a->next_[chrToIdx(c)], start + 1 + chrToIdx(c), f);
			}
	}
#endif // _DEBUG
};     // ff::trie
}      // ff
#endif // __FF_TRIE__
