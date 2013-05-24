#include <cstdio>
#include "bheap.hpp"
#include <complex>
#include <cmath>

namespace { namespace ff {
	template <typename T>
	const T& min(const T& a1, const T& a2) {
		return a1 < a2 ? a1 : a2;
	}}}

#define NUM_LETTERS 'z'-'a' + 1

template<typename TypeChar = char, typename TypeData = int>
class LittleTrie {
#define nil NULL
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
			int i = chrToIdx(tolower(*s));
			if (!c->next[i])
				c->next[i] = new node(tolower(*s), NULL);
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

	void spellcheck(TypeChar* word) const {
		char result[256] = {0}; 
		spellcheck_r(word, &root, 2, 2, result, 0);
		printf(result);
	}

	TypeChar* spellcheck(TypeChar* word, TypeChar* result) {
		spellcheck_i(word, &root, 5, 5, result);
		return result;
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
		TypeChar ch[NUM_LETTERS];
		int index;
		checkst(TypeChar* _w = 0, const node* _c = 0, int _d = 0, int _i = 0, int _index = 0, char _ch = 0)
			: word(_w), c(_c), deletions(_d), insertions(_i), index(_index) { memset(ch, 0, NUM_LETTERS); }
	};

	struct checkst_pred {
		bool operator()(const checkst& a1, const checkst& a2) const
		{ return a1.insertions + a1.deletions + abs((float)(strlen(a1.word) - strlen(a1.ch))) > a2.insertions + a2.deletions + abs((float)(strlen(a1.word) - strlen(a1.ch))); }
	};


	int generate_checkst(const checkst& input, checkst* output) const {
		if (!output) return 0;
		int num = 0;

		const node* c = input.c;
		TypeChar* word = input.word;
		int insertions = input.insertions;
		int deletions  = input.deletions;

		// if we have hit
		if (*input.word)
			if (const node* b = c->next[chrToIdx(word[0])]) {
				output[num].c = b;
				output[num].word = word+1;
				output[num].deletions  = deletions;
				output[num].insertions = insertions;

				if (input.index >= 0) {
					memcpy(output[num].ch, input.ch, input.index+1);
					output[num].index = input.index + 1;
				}
				output[num].ch[output[num].index] = word[0];
				num++;
			}
		
		// suppose that word[0] is inserted symbol
		if (insertions > 0 && input.index > 0) {
			output[num].c = c;
			output[num].word = word+1;
			output[num].deletions  = deletions;
			output[num].insertions = insertions - 1;
			output[num].index = input.index;

			if (input.index > 0)
				memcpy(output[num].ch, input.ch, input.index+1);

			num++;
		}

		// suppose that c->next[i] was deleted from word
		if (deletions > 0) {
			for (int i = 0; i != NUM_LETTERS; ++i) {
				if ( c->next[i] ) {
					output[num].c = c->next[i];
					output[num].word = word;
					output[num].deletions  = deletions - 1;
					output[num].insertions = insertions;

					if (input.index >= 0) {
						memcpy(output[num].ch, input.ch, input.index+1);
						output[num].index = input.index + 1;
					}
					output[num].ch[output[num].index] = (char)i + 'a';
					num++;
				}
			}
		}

		return num;
	}

	bool spellcheck_i(TypeChar* word, const node* c, int deletion, int insertions,  char* result) const {
		ff_bheap<checkst, checkst_pred> b;
		b.insert(checkst(word, c, deletion, insertions, -1));
		checkst out[NUM_LETTERS+2];
		checkst chk;
		int n = 0;
		do {
			chk = b.pop();

			n = generate_checkst(chk, out);
			for (int i = 0; i != n; ++i)
				b.insert(out[i]);

		} while (!chk.c->d && b.getSize());
		if (b.getSize() == 0) {
			printf("ororo");
		}
		memcpy(result, chk.ch, chk.index+1);
		result[chk.index+1] = '\0';

		return (bool)b.getSize() ^ (bool)chk.word[0];
	}

	void print() const {
		FILE* f = fopen("D:\\tex.txt", "wt");
		fprintf(f, "DIGRAPH g {\n");
		print_r(&root, 0, f);
		fprintf(f, "};");
		fclose(f);
	}

	void print_r(const node* a, int start, FILE* f) const {
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next[chrToIdx(c)]) {
				fprintf(f, "%c%d->%c%d;\n", a->ch, start, c, start + 1 + chrToIdx(c));
				print_r(a->next[chrToIdx(c)], start + 1 + chrToIdx(c), f);
			}
	}
#undef nil
};
