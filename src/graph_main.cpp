#include <iostream>
#include <cstdlib>
#include <string>
#include <queue>
#include <exception>
#include <math.h>
#include "graph.hpp"

bool testGraph() {
	ff_digraph<int> graphy;//(10);

	graphy.addNode(0);
	graphy.addNode(1);
	graphy.addNode(2);
	graphy.addNode(3);
	graphy.addNode(4);
// 	graphy.addNode(5);
// 	graphy.addNode(6);
// 	graphy.addNode(7);

	graphy.addConnection(0, 1, 1);
	graphy.addConnection(1, 2, 2);
	graphy.addConnection(1, 3, 1);
	graphy.addConnection(2, 4, 1);
	graphy.addConnection(3, 4, 1);
// 	graphy.addConnection(4, 5, 2);
// 	graphy.addConnection(5, 0, 3);
// 	graphy.addConnection(2, 5, 2);
// 	graphy.addConnection(4, 1, 2);
// 	graphy.addConnection(0, 6, 1);
// 	graphy.addConnection(6, 5, 1);
// 	graphy.addConnection(5, 3, 1);

	size_t s = ff_shortest_path_Astar<ff_digraph<int> >()( graphy, 0, 4 );
	std::cout << s << std::endl;

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