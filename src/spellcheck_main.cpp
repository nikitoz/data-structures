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
#include "spellcheck.hpp"
#define eps 1e-9

#define forn(i, n) for(int i = 0; i < (int)(n); i++)
#define sz(v)((v).size())
#define mp(x, y) make_pair(x, y)
#define ll_t long long

void  ff_Verify(bool b) {
	printf(b ? "OK\n" : "FAIL\n");
}

using namespace std;

void test_spellchecker() {
	LittleTrie<char, int> t;
	int a[5] = {1,2,3,4,5};
	char k = 'n';
	char res[8192] = { 0 };

	t.insert("abc", a+1);
	t.insert("ade", a+2);
	//t.insert("I", a);
	/*t.insert("hablot", a+3);
	t.insert("aalort", a+4);
	t.insert("ablote", a);*/
/*
	t.spellcheck("aalot", res);
	ff_Verify(strcmp("aalot", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("ablot", res);
	ff_Verify(strcmp("ablot", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("arlot", res);
	ff_Verify(strcmp("aalot", res) == 0 || strcmp("ablot", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("tablot", res);
	ff_Verify(strcmp("hablot", res) == 0 || strcmp("ablot", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("ablrte", res);
	ff_Verify(strcmp("ablote", res) == 0);
	memset(res, 0, 255);*/

	t.spellcheck("a", res);
	ff_Verify(strcmp("ade", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("are", res);
	ff_Verify(strcmp("ade", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("b", res);
	ff_Verify(strcmp("abc", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("ab", res);
	ff_Verify(strcmp("abc", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("bc", res);
	ff_Verify(strcmp("abc", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("rbk", res);
	ff_Verify(strcmp("abc", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("brk", res);
	ff_Verify(strcmp("abc", res) == 0);
	memset(res, 0, 255);

	t.spellcheck("e", res);
	ff_Verify(strcmp("ade", res) == 0);
	memset(res, 0, 255);
}


void test_spellchecker1() {
	FILE* f = fopen("D:\\words.txt", "rt");
	LittleTrie<char, int> t;
	char ch[256];
	int i = 1;
	/*while (!feof(f)) {
		fscanf(f, "%s", ch);
		if (strlen(ch) > 1)
			t.insert(ch, &i);
	}*/
	for (int i = 0; i < 20; ++i)
	{
		fscanf(f, "%s", ch);
		if (strlen(ch) > 1)
			t.insert(ch, &i);
	}

	t.print();

	char res[256] = {0};
	while (true) {
		scanf("%s", ch);
		t.spellcheck(ch, res);
		printf("maybe %s ?\n", res);

	}
}

int main() {
	test_spellchecker();
	test_spellchecker1();
	char c;
	scanf("%c", &c);
	return 0;
}