
#include "../src/aho_corasick.hpp"
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

	a.build_from(vs.begin(), vs.end(), CharFromStdString());

	a.print();

	std::vector<int> res = a.match<std::vector<int> >("sherst");

	for (int i = 0; i != (int)res.size(); ++i)
		std::cout << res[i] << " ";
	return 0;
}
