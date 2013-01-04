#include <iostream>
#include <cstdlib>
#include <string>
#include <queue>
#include <exception>
#include <math.h>
#include "graph.hpp"

bool testGraph() {
	ff_directed_graph<int> graphy(10);

	graphy.addNode(0);
	graphy.addNode(1);
	graphy.addNode(2);
	graphy.addNode(3);
	graphy.addNode(4);
	graphy.addNode(5);
	graphy.addNode(6);
	graphy.addNode(7);
	/*graphy.addConnection(0, 1, 1);
	graphy.addConnection(1, 2, 3);
	graphy.addConnection(0, 2, 1);
	graphy.addConnection(0, 5, 25);
	graphy.addConnection(2, 5, 23);
	graphy.addConnection(2, 4, 2);
	graphy.addConnection(4, 5, 10);
	graphy.addConnection(4, 6, 45);
	graphy.addConnection(5, 6, 20);*/
	graphy.addConnection(0, 1, 1);
	graphy.addConnection(1, 2, 2);
	graphy.addConnection(2, 3, 3);
	graphy.addConnection(3, 4, 1);
	graphy.addConnection(4, 5, 2);
	graphy.addConnection(5, 0, 3);
	graphy.addConnection(2, 5, 2);
	graphy.addConnection(4, 1, 2);
	graphy.addConnection(0, 6, 1);
	graphy.addConnection(6, 5, 1);
	graphy.addConnection(5, 3, 1);


	a_search_out *arr;
	arr = graphy.fillShortestPath(0, 4);
	int i = 0;
	while(arr != NULL) {
		std::cout << arr->m_index << std::endl;
		arr = arr->m_next;
	}

	/*if (graphy.getCost(1, 2) != 3) throw std::exception("hello");
	if (graphy.getCost(0, 2) != 1) throw std::exception("hello");
	if (graphy.getCost(0, 5) != 25) throw std::exception("hello");
	if (graphy.getCost(2, 5) != 23) throw std::exception("hello");*/

	return true;
}

int main() {
	/*if (!testHeap())
		std::cout << "!!! Heap test failed !!!" << std::endl;*/

	if (!testGraph())
		std::cout << "!!! Graph test failed !!!" << std::endl;

	std::cin.get();
	return 0;
}