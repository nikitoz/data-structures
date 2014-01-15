#include "../src/aho_corasick.hpp"
#include "../_tests/aho_corasick_test.h"
#include <vector>
#include <iostream>


struct CharFromStdString {
	const char* operator()(const std::string& str) const {
		return str.c_str();
	}
};

int main() {
	ff::aho_corasick  a;
	std::vector<std::string> vs;
	vs.push_back("his");
	vs.push_back("she");
	vs.push_back("he");
	vs.push_back("her");
	vs.push_back("er");
	vs.push_back("e");
	vs.push_back("r");

	a.build_from(vs.begin(), vs.end(), CharFromStdString());

	a.print();

	std::vector<int> res = a.match< std::vector<int> >("sher");

	for (int i = 0; i != (int)res.size(); ++i)
		std::cout << res[i] << " ";

	aho_printer printer(a);
	printer.print();
	return 0;
}
