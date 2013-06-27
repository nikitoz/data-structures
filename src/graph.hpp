#ifndef __FF_GRAPH__
#define __FF_GRAPH__

#include "bheap.hpp"
#include <vector>
#include <map>
#include <functional>
#include <stack>

namespace ff { namespace {

static const size_t INFINITY = (size_t)-1;

/*
    Graph directed edge
*/

template <typename T>
struct diedge
{
	diedge() 
		: _pFrom(NULL), _pTo(NULL), _cost(0)
	{ }

	diedge(T* pFrom, T* pTo, size_t cost) 
		: _pTo(pTo), _pFrom(pFrom), _cost(cost)
	{ }

	size_t to() const
	{ return _pTo->_index; }

	size_t from() const
	{ return _pFrom->_index; }

	bool operator<(const diedge& de) const
	{ return _cost < de._cost; }

	bool operator>(const diedge& de) const
	{ return _cost > de._cost; }

	T* _pFrom;
	T* _pTo;
	size_t _cost;
};

/*
    Graph node
*/

template <typename T>
struct gnode
{
	typedef gnode<T>                  node_t;
	typedef diedge<node_t>            edge_t;
	typedef std::map<size_t, edge_t > edges_t;
	typedef typename size_t			  index_t;

	gnode(T node, size_t index) 
		: _data(node), _index(index)
	{ }

	gnode() 
		: _index(INFINITY)
	{ }

	bool bad() const
	{ return _index == INFINITY; }

	index_t _index;
	T	    _data;
	edges_t _edges; // out from this node
};
} // anonymous-namespace
/*
    General graph search
*/
template 
<
      typename TG  // Graph type
	, typename TP  // Predicate returns true when we're done
	, typename TC  // Container for internal loop
>
struct graph_search
{
	typename TG::node_t operator()( const TG& g, size_t src)
	{
		std::vector<bool> visited(g.count(), false);
		TP is_destination;
		TC container;
		container.push(src);
		visited[src] = true;
		do
		{
			size_t current = container.top();container.pop();

			if (is_destination(g.node(current)))
				return g.node(current);

			for (TG::edges_t::const_iterator it = g.edges(current).begin(); it != g.edges(current).end(); ++it)
				if (visited[it->second.to()] == false)
				{
					visited[it->second.to()] =  true;
					container.push(it->second.to());
				}
		} 
		while (!container.empty());
		return TG::node_t();
	}
};

/*
	Container for bfs
	just a wrapper on std::queue
*/

template <typename T>
struct bfs_container : private std::queue<typename T::index_t>
{
	typedef typename T::index_t index_t;
	using std::queue<index_t>::pop;
	using std::queue<index_t>::push;
	using std::queue<index_t>::empty;

	index_t top()
	{
		return std::queue<index_t>::front();
	}
};

/*
	Container for dfs
	wraps std::stack
*/

template <typename T>
struct dfs_container : private std::stack<typename T::index_t>
{
	typedef typename T::index_t index_t;
	using std::stack<index_t>::pop;
	using std::stack<index_t>::push;
	using std::stack<index_t>::top;
	using std::stack<index_t>::empty;
};

/*
	General search in graph
*/

template 
<
      typename TG // Graph class
    , typename TP // Search finishing condition
    , typename TC // Internal usage container
>
struct xfs
{
	typename TG::node_t operator()( const TG& g, typename TG::index_t from ) const
	{
		return graph_search<TG, TP, TC> () (g, from);
	}
};

/*
	Depth-first search algo
*/

template 
<
      typename TG // Graph class
    , typename TP // Search finishing condition
>
struct dfs
{
	typename TG::node_t operator()( const TG& g, typename TG::index_t from ) const
	{ 
		return xfs<TG, TP, dfs_container<TG> > () (g, from); 
	}
};

/*
	Breadth-first search algo
*/

template 
<
      typename TG // Graph class
    , typename TP // Search finishing condition
>
struct bfs
{
	typename TG::node_t operator()( const TG& g, typename TG::index_t from ) const
	{ 
		return xfs<TG, TP, bfs_container<TG> > () (g, from); 
	}
};

/*
	Functional object for shortest path algo (uses A* inside)
*/

template 
<
      typename TG    // Graph class
    , typename TEP   // Edge predicate
>
struct shortest_path_astar
{
	size_t operator()( const TG& g, size_t src, size_t dst ) const
	{
		if (src == dst) return 0;
		std::vector<size_t> cost(g.count(), INFINITY);
		std::vector<size_t> from(g.count(), INFINITY);
		cost[src] = 0;
		bheap<TG::edge_t, TEP> bheap;
		size_t current = src;
		do
		{
			for (TG::edges_t::const_iterator it = g.edges(current).begin(); it != g.edges(current).end(); ++it)
				if (cost[it->second.to()] == INFINITY)
					bheap.insert(it->second);

			if (bheap.empty())
				return INFINITY;

			TG::edge_t edge = bheap.pop();
			cost[edge.to()] = edge._cost + cost[edge.from()];
			from[edge.to()] = edge.from();
			current         = edge.to();
		}
		while (dst != current);
		return cost[dst];
	}
};

/*
	Digraph class
*/

template
<
	typename T = int // Some associated with node data
>
class digraph
{
public:
	typedef typename gnode<T>		 node_t;
	typedef std::vector<node_t>		 nodes_t;
	typedef typename node_t::edge_t	 edge_t;
	typedef typename node_t::edges_t edges_t;
	typedef typename node_t::index_t index_t;

	friend shortest_path_astar< digraph<T>, std::less<edge_t> >;

	digraph() { }

	digraph(size_t count)
	{ _nodes.reserve(count); }

	size_t add(const T& data)
	{
		_nodes.push_back(node_t(data, _nodes.size()));
		return _nodes.size() - 1;
	}

	void connect(size_t from, size_t to, int cost)
	{
		if (from < _nodes.size() && to < _nodes.size() && to != from)
			_nodes[from]._edges[to] = edge_t(&_nodes[from], &_nodes[to], cost);
	}

	size_t count() const
	{ return _nodes.size();	}

	node_t node(size_t index) const
	{ return _nodes[index]; }

	const nodes_t& nodes() const
	{ return _nodes; }

	bool connected(size_t from, size_t to) const
	{ return edges(from).find(to) != edges(from).end();	}

	edge_t edge(size_t from, size_t to) const
	{ return _nodes[from]._edges[to]; }

	const edges_t& edges(size_t from) const
	{ return _nodes[from]._edges; }

private:
	nodes_t _nodes;
};// digraph
} // ff
#endif // ___FF_GRAPH__