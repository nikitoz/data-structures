#ifndef __FFTEST_H__
#define __FFTEST_H__
#include <iostream>

namespace ff {
struct test {
	static void  verify(bool b, char* ch = 0) {
		std::cout << (b ? "OK" : "FAIL ") << ( b ? "" : ch) << std::endl;
	}
	static void say(char* ch = 0) {
		std::cout << ch << std::endl;
	}
	static void tick() {
		char ch;
		std::cin >> ch;
	}
};

void assert(bool expression) {
	if (!expression)
		std::cerr << "ASSERT failed" << std::endl;
}
}

#ifdef _DEBUG
#define FFASSERT ff::assert
#else
#define FFASSERT ;
#endif // _DEBUG
#endif //__FFTEST_H__