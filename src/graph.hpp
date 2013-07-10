#ifndef __FF_GRAPH__
#define __FF_GRAPH__

#include "bheap.hpp"
#include <vector>
#include <map>
#include <functional>
#include <stack>
#include <set>

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
	typedef typename size_t			  index_t;
	typedef std::map<index_t, edge_t> edges_t;

	gnode(T node, size_t index) 
		: _data(node), _index(index)
	{ }

	gnode() 
		: _index(INFINITY)
	{ }

	bool bad() const
	{ return _index == INFINITY; }

	size_t ecount() const
	{ return _edges.size(); }

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
		std::vector<bool> visited(g.ncount(), false);
		TP is_destination;
		TC container;
		container.push(src);
		visited[src] = true;
		do
		{
			size_t current = container.top(); container.pop();

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
		std::vector<size_t> cost(g.ncount(), INFINITY);
		std::vector<size_t> from(g.ncount(), INFINITY);
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

struct coloring_result
{
	coloring_result(std::vector<int> node_colors, size_t color_count, bool optimal, std::vector<int> more_optimal = std::vector<int>())
		: _is_optimal(optimal), _node_colors(node_colors), _color_count(color_count), _more_optimal(more_optimal), _is_valid(true)
	{ }

	coloring_result()
		: _is_valid(false)
	{ }

	size_t ccount() const
	{ return _color_count; }

	std::vector<int> nodes_colors() const
	{ return _node_colors; }

	bool optimal() const
	{ return _is_optimal; }

	bool valid() const
	{ return _is_valid; }

private:
	std::vector<int> _node_colors;
	size_t _color_count;
	bool _is_optimal;
	std::vector<int> _more_optimal;
	bool _is_valid;
};

/*
	Descartes graph builder
*/

template
<
	  typename TG
	, typename coord
>
struct descartes_graph_builder
{
	typedef typename TG::index_t index_t;

	void operator()(TG& g, std::istream& input_stream) const
	{
		index_t index = 0;
		coord xs[];
		coord ys[];
		while (input_stream.good())
		{
			input_stream >> xs[index] >> ys[index];
			g.add(index++);
		}

		for (size_t i = 0; i != index; ++i)
			for (size_t j = i + 1; j != index; ++j)
				g.connect(i, j, length(xs[i], ys[i], xs[j], ys[j]));
	}

private:

	coord length(coord x1, coord y1, coord x2, coord y2) const
	{
		return static_cast<coord>(sqrt( (double)pow((double)x1-x2, 2), (double)pow((double)y1-y2, 2) ));
	}
};

/*
	Functional object for finding graph coloring, using branch and bound approach
	Quick and dirty implementation. Abandoned.
*/

template 
<
      typename TG
>
struct coloring_b_and_b
{
	coloring_b_and_b( )
	{ }

	coloring_result operator()( const TG& g ) const
	{
		if (g.ncount() == 0)
			return coloring_result();

		size_t init_ubound = chromtic_number_estimation_not_so_pessimistic(g);
		size_t ubound = chromtic_number_estimation_not_so_pessimistic(g);
		ints_t colours(g.ncount(), NO_COLOUR);
		int colored = 0;
		eSolution solution_status = Infeasible;

		size_t opt_bound = 7;//ubound/2;
		do {
			ints_t opt_colours(g.ncount(), NO_COLOUR);
			int    opt_colored = 0;

			_possible_colours = make_possible_colours(opt_bound);
			solution_status = solve_r(g, 0, opt_colours, opt_colored);
			if (solution_status == Feasible)
			{
				colours = opt_colours;
				colored = opt_colored;
				ubound  = opt_bound;
			}
			opt_bound = distinct_colours(colours) - 1;
		} while (false);

		switch (solution_status)
		{
			case Feasible :
			{
				ints_t opt_colours(g.ncount(), NO_COLOUR);
				int    opt_colored = 0;
				size_t unique_colours = distinct_colours(colours);
				_possible_colours = make_possible_colours(unique_colours - 1);
				eSolution solution_status2 = solve_r(g, 0, opt_colours, opt_colored);
				return coloring_result(colours, unique_colours, Infeasible == solution_status2, opt_colours);
			}
			case Infeasible :
				throw std::runtime_error("Unexpected result, should be possible to find solution");

			case Maybe:
				/*In this case we do have more than 1 connected component, this will be handled some time in the future*/
				break;
		}

		return coloring_result();
	}

private:

	typedef std::vector<int> ints_t;
	typedef typename TG::index_t index_t;
	typedef typename std::vector<index_t> indices_t;
	static const int NO_COLOUR = -1;

	enum eSolution {Feasible, Infeasible, Maybe};

	/*
		Simple recursive B&B algo. To be rewritten with iterations.
	*/

	eSolution solve_r(const TG& g, const index_t index, ints_t& colours, int& colored) const
	{
		std::set<int> possible_colours(_possible_colours);
		std::vector<index_t> uncolored_vertices;

		for (typename TG::edges_t::const_iterator it = g.edges(index).begin();
			it != g.edges(index).end() && !possible_colours.empty(); ++it)
		{
			if (colours[it->second.to()] == NO_COLOUR)
				uncolored_vertices.push_back(it->second.to());
			else if (colours[index] == NO_COLOUR)
				possible_colours.erase(colours[it->second.to()]);
		}

		std::sort(uncolored_vertices.begin(), uncolored_vertices.end(), node_sort_pred(g));
		
		if (possible_colours.empty())
			return Infeasible; // Upper bound is exceeded

		std::set<int>::const_iterator current_color_to_check = possible_colours.begin();
		colours[index] = *current_color_to_check;
		colored ++;
		
		if (colored == g.ncount())
			return Feasible; // Everything is colored

// 		ints_t init_colours = colours;
// 		int    init_colored = colored; 

		for (indices_t::iterator it = uncolored_vertices.begin(); it != uncolored_vertices.end(); ++it) {
			if (colours[*it] != NO_COLOUR)
			{
				uncolored_vertices.erase(it);
			}
		}

		ints_t init_colours = colours;
		int    init_colored = colored;

		indices_t::iterator it = uncolored_vertices.begin();
		while (it != uncolored_vertices.end())
		{
// 			if (colours[*it] != NO_COLOUR)
// 			{
// 				++it;
// 				continue;
// 			}

			ints_t init_colours = colours;
			int    init_colored = colored;
			eSolution isSolved = solve_r(g, *it, colours, colored );
			switch (isSolved)
			{
				case Feasible  : return Feasible;
				case Maybe : ++it;
					init_colours = colours;
					init_colored = colored;
					break;
				case Infeasible :
					{
						if (++current_color_to_check == possible_colours.end())
							return Infeasible;

						init_colours[index] = *current_color_to_check;
						colours = init_colours;
						colored = init_colored;
						//it = uncolored_vertices.begin();
						//colored = init_colored;
						//return Infeasible;
					} break;
			}
		}
		return Maybe;
	}

	size_t distinct_colours(std::vector<int>& colours) const
	{
		std::set<int> cols;
		for (std::vector<int>::const_iterator it = colours.begin(); it != colours.end(); ++it)
			cols.insert(*it);
		return cols.size();
	}

	size_t chromtic_number_estimation_pessimistic(const TG& g) const
	{
		size_t max_num = 0;
		for ( TG::nodes_t::const_iterator it  = g.nodes().begin(); it != g.nodes().end(); ++it)
		{
			size_t pretendent = it->_edges.size();
			max_num = max_num < pretendent ? pretendent : max_num;
		}
		return max_num;
	}

	size_t chromtic_number_estimation_not_so_pessimistic(const TG& g) const
	{
		size_t sum = 0;
		for ( TG::nodes_t::const_iterator it  = g.nodes().begin(); it != g.nodes().end(); ++it)
			sum += it->_edges.size();
		return sum/g.ncount();
	}

	struct node_sort_pred
	{
		node_sort_pred(const TG& g) : _g(g) {}

		bool operator()(typename TG::index_t a1, typename TG::index_t a2) const
		{ return _g.node(a1).ecount() > _g.node(a2).ecount(); }

		const TG& _g;
	};

	struct bounder
	{
		bounder()
			: _lbound(-1)
		{ }

		int operator()() const
		{
			++_lbound;
			return _lbound;
		}
	private:
		mutable int _lbound;
	};

	static std::set<int> make_possible_colours(size_t ubound)
	{
		std::set<int> possible_colours;
		std::generate_n(std::inserter(possible_colours, possible_colours.begin()), ubound, bounder());
		return possible_colours; 
	}

	mutable std::set<int> _possible_colours;
}; // coloring_b_and_b

/*
	TSP solver. Hamiltonian cycle
*/

template
<
	typename TG
>
struct tsp
{
	typedef typename TG::index_t index_t;

	std::vector<index_t> operator()(const TG& g) const
	{
		
	}
};

/*
	Undirected graph class
*/

template
<
	  typename TNode = int // Some associated with node data
	, typename TLength = size_t // edge length type
>
struct graph : public digraph<T, TLength>
{
	void connect(size_t from, size_t to, T cost)
	{
		digraph<TNode, TLength>::connect(from, to, cost);
		digraph<TNode, TLength>::connect(to, from, cost);
	}
};


/*
	Digraph class
*/

template
<
	  typename TNode = int // Some associated with node data
	, typename TLength = size_t // edge length type
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

	size_t cost(size_t from, size_t to) const
	{ return shortest_path_astar< digraph<T>, std::less<edge_t> >()(*this, from, to); }

	coloring_result colours() const
	{ return coloring_b_and_b< digraph<T> >()( *this );	}

	size_t ncount() const
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

public:

	/*
		Testing helper stuff
	*/

#ifdef _DEBUG
	bool test_CheckColoring(coloring_result& cr) const
	{
		std::vector<int> node_colors = cr.nodes_colors();
		for ( nodes_t::const_iterator it  = _nodes.begin(); it != _nodes.end(); ++it)
			for (edges_t::const_iterator ie = it->_edges.begin(); ie != it->_edges.end(); ++ie)
				if (node_colors[ie->second.to()] == node_colors[ie->second.from()])
					throw std::exception("Incorrect coloring");
	}
#endif // #ifdef _DEBUG
};// digraph
} // ff
#endif // ___FF_GRAPH__