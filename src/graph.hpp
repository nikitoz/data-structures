#include "bheap.hpp"
#include <vector>
#include <map>
#include <functional>
#include <stack>

namespace {
	static const size_t INFINITY = (size_t)-1;
	//=========================================================================
	// Directed edge of a graph
	//=========================================================================
	template <typename TNode>
	struct diedge {
		TNode* _pFrom;
		TNode* _pTo;
		size_t _cost;

		diedge() 
			: _pFrom(NULL), _pTo(NULL), _cost(0) { }

		diedge(TNode* pFrom, TNode* pTo, size_t cost) 
			: _pTo(pTo), _pFrom(pFrom), _cost(cost) { }

		size_t to() const
		{ return _pTo->_index; }

		size_t from() const
		{ return _pFrom->_index; }

		bool operator<(const diedge& de) const
		{ return _cost < de._cost; }

		bool operator>(const diedge& de) const
		{ return _cost > de._cost; }
	};
	//=========================================================================
	// Node of a graph
	//=========================================================================
	template <typename T>
	struct gnode {
		typedef gnode<T>                  node_t;
		typedef diedge<node_t>            edge_t;
		typedef std::map<size_t, edge_t > edges_t;
		typedef typename size_t			  index_t;

		gnode(T node, size_t index) 
			: _data(node), _index(index) { }

		gnode() 
			: _index(INFINITY) { }

		bool bad() const
		{ return _index == INFINITY; }

		index_t _index;
		T	    _data;
		edges_t _edges;// out from this node
	};
}
//=========================================================================
// Graph search algo
//=========================================================================
template <typename TG  // Graph type
		, typename TP  // Predicate returns true when we're done
		, typename TC  // Container for internal loop
		, typename TCG // Functor that retrieves value from container 
		 >
struct ff_graph_search {
	typename TG::node_t operator()( const TG& digraph, size_t src) {
		bool* visited = new bool[digraph.count()];
		std::memset(visited, false, digraph.count()*sizeof(bool));
		TP  predicate;
		TCG top;

		TC q;
		q.push(src);
		visited[src] = true;
		size_t current = src;
		bool found = false;

		do {
			current = top(q); q.pop();
			for (TG::edges_t::const_iterator it = digraph.edges(current).begin(); it != digraph.edges(current).end(); ++it) {
				if (!visited[it->second.to()]) {
					q.push(it->second.to());
					visited[it->second.to()] = true;
				}
			}
		} while (!q.empty() || !(found = predicate(digraph.node(current))));
		return found ? digraph.node(current) : TG::node_t();
	}
};
//=========================================================================
// Breadth-first search algo
//=========================================================================
template <typename TG, typename TP>
struct ff_bfs {
	typedef typename TG::index_t index_t;
	typedef std::queue<index_t> container_t; 
	struct bfs_container_pop {
		index_t operator()(container_t& q) const
		{ return q.front();	}
	};
	typename TG::node_t operator()( const TG& digraph, index_t src ) const
	{ return ff_graph_search<TG, TP, container_t, bfs_container_pop> () (digraph, src);	}
};
//=========================================================================
// Depth-first search algo
//=========================================================================
template <typename TG, typename TP>
struct ff_dfs {
	typedef typename TG::index_t index_t;
	typedef std::stack<index_t> container_t; 
	struct bfs_container_pop {
		index_t operator()(container_t& q) const
		{ return q.top(); }
	};
	typename TG::node_t operator()( const TG& digraph, index_t src ) const
	{ return ff_graph_search<TG, TP, container_t, bfs_container_pop > () (digraph, src); }
};
//=========================================================================
// Functional object for shortest path algo (uses A* inside)
//=========================================================================
template <typename TG>
struct ff_shortest_path_astar {

	struct edge_predicate {
		bool operator()(const typename TG::edge_t& ge1, const typename TG::edge_t& ge2) const
		{ return ge1 < ge2; }
	};

	size_t operator()( const TG& digraph, size_t src, size_t dst ) const {
		size_t* cost = new size_t[digraph.count()];
		size_t* esrc = new size_t[digraph.count()];

		std::memset(cost, INFINITY, digraph.count()*sizeof(size_t));
		std::memset(esrc, INFINITY, digraph.count()*sizeof(size_t));

		cost[src] = 0;
		ff_bheap<TG::edge_t, edge_predicate > bheap;
		size_t current = src;

		while (current != dst) {
			for (TG::edges_t::const_iterator it = digraph.edges(current).begin(); it != digraph.edges(current).end(); ++it)
					if (cost[it->second.to()] == INFINITY)
						bheap.insert(it->second);

			if (bheap.empty()) break;
			TG::edge_t edge = bheap.pop();

			cost[edge.to()] = edge._cost + cost[edge.from()];
			esrc[edge.to()] = edge.from();
			current         = edge.to();
		}
		size_t res = cost[dst];
		delete[] cost;
		delete[] esrc;
		return res;
	}
};
//=========================================================================
// Class representing directed graph
//=========================================================================
template<typename TNode>
class ff_digraph {
	friend ff_shortest_path_astar< ff_digraph<TNode> >;

public:
	typedef typename gnode<TNode>		node_t;
	typedef std::vector<node_t>			nodes_t;
	typedef typename node_t::edge_t		edge_t;
	typedef typename node_t::edges_t    edges_t;
	typedef typename node_t::index_t	index_t;

	ff_digraph() { }

	ff_digraph(size_t count)
	{ _nodes.reserve(count); }

	size_t addNode(const TNode& data) {
		_nodes.push_back(node_t(data, _nodes.size()));
		return _nodes.size() - 1;
	}

	void addConnection(size_t from, size_t to, int cost) {
		if (from >= _nodes.size() || to >= _nodes.size() || to == from)
			return;

		_nodes[from]._edges[to] = edge_t(&_nodes[from], &_nodes[to], cost);
	}

	size_t count() const
	{ return _nodes.size();	}

	node_t node(size_t index) const
	{ return _nodes[index]; }

	const nodes_t& nodes() const
	{ return _nodes; }

	bool hasConnection(size_t from, size_t to) const
	{ return edges(from).find(to) != edges(from).end();	}

	edge_t edge(size_t from, size_t to) const
	{ return _nodes[from]._edges[to]; }

	const edges_t& edges(size_t from) const
	{ return _nodes[from]._edges; }

private:
	nodes_t _nodes;
};