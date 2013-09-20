#ifndef __FF__TRIE__
#define __FF_TRIE__
#include <exception>
#include <stack>

namespace ff {

template
<
	  typename TypeChar = char
	, typename TypeData = int
	, int FIRST_LETTER = 'a'
	, int NUM_LETTERS =  'z'-'a' + 1
>
class trie
{
private:
	struct node
	{
		TypeChar  ch_;
		TypeData  d_;
		char      child_count_;
		node*     next_[NUM_LETTERS];
		bool      is_leaf_;
		node (TypeChar c = TypeChar(), TypeData d = 0)
			: ch_(c)
			, d_ (d)
			, child_count_(0)
			, is_leaf_(false)
		{ std::memset(next_, 0, sizeof(next_)); }
	} root_;

public:
	trie()
	{ }

	void insert(const TypeChar* str, const TypeData& data) {
		z_insert(&root_, str, data);
	}

	const TypeData& try_get(const TypeChar* str) const {
		node* n = find(str);
		if (0 == n || false == n->is_leaf_)
			throw std::exception("value not found");
		return n->d_;
	}

	TypeData& get(const TypeChar* str) {
		return z_find_insert(&root_, str)->d_;
	}

	void remove(const TypeChar* str) {
		if (0 == str)
			return;

		std::stack<node*> st;
		node* c  = &root_;
		for (const TypeChar* s = str; *s; ++s) {
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
				const TypeChar t = c->ch_;
				delete c;
				c = st.top();
				c->next_[chrToIdx(tolower(t))] = 0;
			} else break;
		}
	}

private:

	node* z_find(node* root, const TypeChar* str) {
		if (0 == str)
			return 0;
		node* c  = root_;
		for (const TypeChar* s = str; *s; ++s) {
			const int i = chrToIdx(*s);
			if (c->next_[i])
				c = c->next_[i];
			else
				return 0;
		}
		return c;
	}

	node* z_find_insert(node* root, const TypeChar* str) {
		if (0 == str || 0 == root)
			return 0;
		node* c  = root;
		for (const TypeChar* s = str; *s; ++s) {
			const int i = chrToIdx(*s);
			if (c->next_[i])
				c = c->next_[i];
			else
				return z_insert(c, s, TypeData());
		}
		c->is_leaf_ = true;
		return c;
	}

	node* z_insert(node* root, const TypeChar* str, const TypeData& data) {
		if (0 == str || 0 == root)
			return 0;
		node* c  = root;
		for (const TypeChar* s = str; *s; ++s) {
			const int i = chrToIdx(tolower(*s));
			if (!c->next_[i]) {
				++c->child_count_;
				c->next_[i] = new node(tolower(*s), 0);
			}
			c = c->next_[i];
		}
		c->d_       = data;
		c->is_leaf_ = true;
		return c;
	}

	int chrToIdx(TypeChar ch) const
	{ return ch - FIRST_LETTER;}

public:

#if _DEBUG
	bool test_count() {
		return test_count_r(&root_);
	}

	bool test_count_r(node* root) {
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
		print_r(&root_, 0, f);
		fprintf(f, "};");
		fclose(f);
	}

	void print_r(const node* a, int start, FILE* f) const {
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