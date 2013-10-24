#include "value_ordered_map.hpp"
#include <string>
#include <iostream>

int main() {
	value_ordered_map<std::string, int> mm;
	mm.set("192.168.0.1", 3);
	mm.set("192.168.0.4", 4);
	mm.set("192.168.0.3", 3);
	mm.set("192.168.0.5", 5);
	mm.set("192.168.0.55", 5);
	mm.set("192.168.55.5", 5);
	mm.set("192.168.0.1", 1);

	std::pair<value_ordered_map<std::string, int>::sorted_by_value_iterator, 
		value_ordered_map<std::string, int>::sorted_by_value_iterator> pr = mm.sorted_by_value_range();

	while (pr.first != pr.second) {
		std::cout << pr.first->first << " " << pr.first->second << std::endl;
		pr.first++;
	}

	return 0;
}