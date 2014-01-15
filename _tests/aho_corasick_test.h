#ifndef __FF__AHO__CORASICK__TEST__
#define __FF__AHO__CORASICK__TEST__
#include <map>
#include <cstdio>
#include "../src/aho_corasick.hpp"
class aho_printer {
public:
	typedef ff::aho_corasick::node_t node_t;
	const ff::aho_corasick& aho_;

	aho_printer(const ff::aho_corasick& aho) : aho_(aho) {}

	bool test_count() {
		return test_count_r(aho_.root_);
	}

	bool test_count_r(node_t* root) {
		if (0 == root) return true;
		int sum = 0;
		for (int i = 0; i != aho_.NUM_LETTERS; ++i)
			if (0 != root->next_[i]) ++sum;

		if (sum != root->child_count_) return false;

		bool res = true;
		for (int i = 0; i != aho_.NUM_LETTERS; ++i)
			if (0 != root->next_[i]) res &= test_count_r(root->next_[i]);
		return res;
	}

	void print() const {
		std::map<const node_t*, int> maps;
		FILE* f = fopen("D:\\tex.txt", "wt");
		fprintf(f, "DIGRAPH g {\n");
		fill_cache_map_r(aho_.root_, 0, maps);
		print_r(aho_.root_, 0, f, maps);
		fprintf(f, "};");
		fclose(f);
	}

	void fill_cache_map_r(const node_t* a, int start, std::map<const node_t*, int>& maps) const {
		maps[a] = start;
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next_[aho_.chrToIdx(c)]) {
				maps[a->next_[aho_.chrToIdx(c)]] = start + 1 + aho_.chrToIdx(c);
				fill_cache_map_r(a->next_[aho_.chrToIdx(c)], start + 1 +aho_. chrToIdx(c), maps);
			}
	}

	void print_r(const node_t* a, int start, FILE* f, std::map<const node_t*, int>& maps) const {
		for (char c = 'a'; c < 'z'; ++c)
			if (a->next_[aho_.chrToIdx(c)]) {
				fprintf(f, "%c%d->%c%d;\n", a->ch_, start, c, start + 1 + aho_.chrToIdx(c));
				print_r(a->next_[aho_.chrToIdx(c)], start + 1 + aho_.chrToIdx(c), f, maps);
			}

			fprintf(f, "%c%d->%c%d [color=\"red\"];\n", a->ch_, start, a->suffix_->ch_, maps[a->suffix_]);
	}
};

#endif // __FF__AHO__CORASICK__TEST__