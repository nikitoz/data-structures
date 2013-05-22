#include <algorithm>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <set>
#include <cassert>
#include <ctime>
#include <functional>
#include <cmath>
#include <memory.h>
#define eps 1e-9

#define forn(i, n) for(int i = 0; i < (int)(n); i++)
#define sz(v)((v).size())
#define mp(x, y) make_pair(x, y)
#define ll_t long long
#define ff_Verify(x, y) printf(x == y ? "OK" : "FAIL");

using namespace std;

namespace ff {
	template <typename T>
	const T& min(const T& a1, const T& a2) {
		return a1 < a2 ? a1 : a2;
	}
}

#define NUM_LETTERS 'z'-'a' + 1

template<typename TypeChar = char, typename TypeData = int>
class LittleTrie {
private:
	int chrToIdx(char ch) const { return ch - 'a';}
	struct node {
		TypeChar ch;
		TypeData* d;
		node* next[NUM_LETTERS];
		node(TypeChar _c = TypeChar(), TypeData* _d = NULL) : ch(_c), d(_d) { memset(next, 0, sizeof(next)); }
	};
	node root;

public:
	LittleTrie() { }

	void insert(TypeChar* str, TypeData* data) {
		node* c  = &root;
		for (TypeChar* s = str; *s; ++s) {
			int i = chrToIdx(*s);
			if (!c->next[i])
				c->next[i] = new node(*s, NULL);
			c = c->next[i];
		}
		c->d = data;
	}

	TypeData* find(TypeChar* str) const {
		node* c  = &root;
		for (TypeChar* s = str; *s; ++s) {
			int i = chrToIdx(*s);
			if (!c->next[i])
				return NULL;
			else
				c = c->next[i];
		}
		return c->d;
	}

	static const int MAX_DEL = 2;
	static const int MAX_INS = 2;
	//static const int MAX_MISS = MAX_DEL + MAX_INS + 1;


	void spellcheck(TypeChar* word) const {
		char result[256] = {0}; 
		spellcheck_r(word, &root, 2, 2, result, 0);
		printf(result);
	}


	bool spellcheck_r(TypeChar* word, const node* c, int deletionLeft, int insertionsLeft,  char* result, int resPos) const {
		if (*word == 0 && c->d != 0)
			return true;
		if (*word == 0 && c->d != 0)
			return false;

		if (node* b = c->next[chrToIdx(word[0])]) {
			result[resPos] = word[0];
			if (spellcheck_r(word + 1, b, deletionLeft, insertionsLeft, result, resPos+1))
				return true;
		}

		if (deletionLeft > 0) {
			int l = (int)strlen(word);//ff::min(strlen(word), MAX_MISS);
			for (int i = 0; i < NUM_LETTERS; ++i)
				if (node* b = c->next[i]) {
					result[resPos] = b->ch;
					if (spellcheck_r(word, b, deletionLeft-1, insertionsLeft, result, resPos+1))
						return true;
				}
		}

		if (insertionsLeft > 0) {
			if (spellcheck_r(word+1, c, deletionLeft, insertionsLeft-1, result, resPos))
				return true;
		}
	}

	struct checkst {
		TypeChar* word;
		const node* c;
		int deletions;
		int insertions;
		checkst(TypeChar* _w = 0, const node* _c = 0, int _d = 0, int _i = 0) : word(_w), c(_c), deletions(_d), insertions(_i) { }
	};

	bool checkst_pred(const checkst& a1, const checkst& a2) const {
		return a1.insertions + a1.deletions < a2.insertions + a2.deletions;
	}

	int generate_checkst(const checkst& input, checkst* output, int& nout) const {
		if (!output) return;
		if (!*input.word) return;
		int num = 0;
		node* c = input.c;
		TypeChar* word = input.word;
		if (node* b = c->next[chrToIdx(word[0])]) {
			output[num].c = b;
			output[num].word = word+1;
			output[num].deletions  = input.deletions;
			output[num].insertions = input.insertions;
		}

		return num;
	}

	bool spellcheck_i(TypeChar* word, const node* c, int deletionLeft, int insertionsLeft,  char* result, int resPos) const {

	}

	void print() const {
		FILE* f = fopen("D:\\tex.txt", "wt");
		fprintf(f, "DIGRAPH g {\n");
		print_r(&root, 0, f);
		fprintf(f, "};");
	}

	void print_r(const node* a, int start, FILE* f) const {
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next[chrToIdx(c)]) {
				fprintf(f, "%c%d->%c%d;\n", a->ch, start, c, start + 1 + chrToIdx(c));
				print_r(a->next[chrToIdx(c)], start + 1 + chrToIdx(c), f);
			}
	}
};

int main() {
	LittleTrie<char, int> t;
	int a[5] = {1,2,3,4,5};
	t.insert("aalot", a+1);
	t.insert("ablot", a+2);
	t.insert("hablot", a+3);
	t.insert("aalort", a+4);
	t.insert("ablote", a);
	t.print();

	t.spellcheck("aalot");
	return 0;
}