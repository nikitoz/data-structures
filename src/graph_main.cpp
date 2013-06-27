#include <iostream>
#include <cstdlib>
#include <string>
#include <queue>
#include <exception>
#include <math.h>
#include "graph.hpp"

using namespace ff;

template <typename T>
struct all_true {
	bool operator()(const T&) {
		return true;
	}
};
bool testGraph() {
	digraph<int> graphy;//(10);

	graphy.add(0);
	graphy.add(1);
	graphy.add(2);
	graphy.add(3);
	graphy.add(4);
// 	graphy.add(5);
// 	graphy.add(6);
// 	graphy.add(7);

	graphy.connect(0, 1, 1);
	graphy.connect(1, 2, 2);
	graphy.connect(1, 3, 1);
	graphy.connect(2, 4, 1);
	graphy.connect(3, 4, 1);
// 	graphy.addConnection(4, 5, 2);
// 	graphy.addConnection(5, 0, 3);
// 	graphy.addConnection(2, 5, 2);
// 	graphy.addConnection(4, 1, 2);
// 	graphy.addConnection(0, 6, 1);
// 	graphy.addConnection(6, 5, 1);
// 	graphy.addConnection(5, 3, 1);

	typedef digraph<int> Graph;

	size_t s = shortest_path_astar< Graph, std::less<Graph::edge_t> >()( graphy, 0, 4 );
	std::cout << s << std::endl;

	bfs<Graph, all_true<Graph::node_t> >()(graphy, 0);
	dfs<Graph, all_true<Graph::node_t> >()(graphy, 0);

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