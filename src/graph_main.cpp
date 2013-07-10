#include <iostream>
#include <cstdlib>
#include <string>
#include <queue>
#include <exception>
#include <vector>
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

void courseraTest(const char * path) {
	FILE*f=fopen(path, "rt");
	int N, E;
	fscanf(f, "%d%d", &N, &E);
	typedef digraph<int> Graph;
	Graph graph;
	for (int i = 0; i < N; ++i)
		graph.add(i);
	for (int i = 0; i < E; ++i) {
		int a, b;
		fscanf(f, "%d%d", &a, &b);
		graph.connect(a, b, 1);
		graph.connect(b, a, 1);
	}

	//size_t s = graph.cost(0, N-1);
	coloring_result c = graph.colours();
	std::vector<int> nc = c.nodes_colors();

	printf("%u %d\n", c.ccount(), (int)c.optimal());
	for (std::vector<int>::const_iterator it = nc.begin(); it != nc.end(); ++it)
	{
		printf("%d ", *it);
	}
// 	try {
// 		graph.test_CheckColoring(c);
// 	} catch(std::exception& e) {
// 		printf(e.what());
// 	}
}

void descartesLoadTest(const char* path)
{
	graph graph;
	std::ifstream input_stream(path);
	ff::descartes_graph_builder<tgraph, double> graph_builder;
	graph_builder(graph, input_stream);
	graph.ncount();
}

int main(int argc, char** argv) {
	/*if (!testHeap())
		std::cout << "!!! Heap test failed !!!" << std::endl;*/

// 	if (!testGraph())
// 		std::cout << "!!! Graph test failed !!!" << std::endl;
// 	for (int i = 0 ; i < argc; ++i) {
// 		printf(argv[i]);
// 	}
	descartesLoadTest("d:/data/desc.txt");
	//courseraTest("tmp.data");
	//std::cin.get();
	return 0;
}