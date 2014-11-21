#ifndef __FF_GRAPH__
#define __FF_GRAPH__

#include "bheap.hpp"
#include <vector>
#include <map>
#include <functional>
#include <stack>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iosfwd>

namespace ff { namespace {

static const size_t INFINITY = (size_t)-1;

/*
    Graph directed edge
*/

template <typename T, typename TCost>
struct diedge
{
	diedge() 
		: from_(0), to_(0), cost_(0)
	{ }

	diedge(T* from, T* to, TCost cost) 
		: to_(to), from_(from), cost_(cost)
	{ }

	size_t to() const
	{ return to_->index_; }

	size_t from() const
	{ return from_->index_; }

	bool operator<(const diedge& de) const
	{ return cost_ < de.cost_; }

	bool operator>(const diedge& de) const
	{ return cost_ > de.cost_; }

	T*     from_;
	T*     to_;
	TCost cost_;
}; // diedge

/*
    Graph node
*/

template <typename T, typename TCost>
struct gnode
{
	typedef gnode<T, TCost>           node_t;
	typedef diedge<node_t, TCost>     edge_t;
	typedef typename size_t			  index_t;
	typedef std::map<index_t, edge_t> edges_t;

	gnode(T node, size_t index) 
		: data_(node), index_(index)
	{ }

	gnode() 
		: index_(INFINITY)
	{ }

	bool bad() const
	{ return index_ == INFINITY; }

	size_t ecount() const
	{ return edges_.size(); }

	index_t index_;
	T	    data_;
	edges_t edges_; // out from this node
};// gnode
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
void graph_search ( const TG& g, size_t src)
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
typename TG::node_t xfs ( const TG& g, typename TG::index_t from )
{
	return graph_search <TG, TP, TC> (g, from);
}

/*
	Depth-first search algo
*/

template 
<
      typename TG // Graph class
    , typename TP // Search finishing condition
>
typename TG::node_t dfs ( const TG& g, typename TG::index_t from )
{ 
	return xfs <TG, TP, dfs_container<TG> > (g, from); 
}

/*
	Breadth-first search algo
*/

template 
<
      typename TG // Graph class
    , typename TP // Search finishing condition
>
typename TG::node_t bfs ( const TG& g, typename TG::index_t from )
{ 
	return xfs <TG, TP, bfs_container<TG> > (g, from); 
}

/*
	Functional object for shortest path algo (uses A* inside)
*/

template 
<
      typename TG    // Graph class
    , typename TEP   // Edge predicate
>
size_t shortest_path_astar( const TG& g, size_t src, size_t dst )
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

struct coloring_result
{
	coloring_result(std::vector<int> node_colors, size_t color_count, bool optimal, std::vector<int> more_optimal = std::vector<int>())
		: is_optimal_(optimal), node_colors_(node_colors), color_count_(color_count), more_optimal_(more_optimal), is_valid_(true)
	{ }

	coloring_result()
		: is_valid_(false)
	{ }

	size_t ccount() const
	{ return color_count_; }

	std::vector<int> nodes_colors() const
	{ return node_colors_; }

	bool optimal() const
	{ return is_optimal_; }

	bool valid() const
	{ return is_valid_; }

private:
	std::vector<int> node_colors_;
	std::vector<int> more_optimal_;
	size_t           color_count_;
	bool             is_optimal_;
	bool             is_valid_;
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
			sum += it->edges_.size();
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

template
<
	typename TG
>
void from_text_file_adjecency_list(TG& graph, const char* filename)
{
	std::ifstream in_file(filename);
	if (!in_file.is_open())
		return;
	while (in_file.good()) {
		std::string ln;
		std::getline(in_file, ln);
		std::stringstream ln_stream(ln);
		int src = 0;
		ln_stream >> src;
		int dst = 0;
		while (ln_stream.good()) {
			ln_stream >> dst;
			graph.connect(src - 1, dst - 1, 0);
		}
	}
}

/*
	Digraph class
*/

template
<
	  typename TNode   = int    // Some associated with node data
	, typename TLength = size_t // edge length type
>
class digraph
{
public:
	typedef TLength cost_t;
	typedef typename gnode<TNode, TLength>	 node_t;
	typedef std::vector<node_t>		 nodes_t;
	typedef typename node_t::edge_t	 edge_t;
	typedef typename node_t::edges_t edges_t;
	typedef typename node_t::index_t index_t;

	digraph() { }

	digraph(size_t count)
	{ nodes_.reserve(count); }

	size_t add(const TNode& data)
	{
		nodes_.push_back(node_t(data, nodes_.size()));
		return nodes_.size() - 1;
	}

	void connect(size_t from, size_t to, TLength cost)
	{
		if (from < nodes_.size() && to < nodes_.size() && to != from)
			nodes_[from].edges_[to] = edge_t(&nodes_[from], &nodes_[to], cost);
	}

	/*size_t cost(size_t from, size_t to) const
	{ return ff::shortest_path_astar< digraph<T>, std::less<edge_t> >(*this, from, to); }*/

	coloring_result colours() const
	{ return coloring_b_and_b< digraph<TNode, TLength> >()( *this );	}

	size_t ncount() const
	{ return nodes_.size();	}

	node_t node(size_t index) const
	{ return nodes_[index]; }

	const nodes_t& nodes() const
	{ return nodes_; }

	bool connected(size_t from, size_t to) const
	{ return edges(from).find(to) != edges(from).end();	}

	edge_t edge(size_t from, size_t to) const
	{ return nodes_[from].edges_[to]; }

	const edges_t& edges(size_t from) const
	{ return nodes_[from].edges_; }

	bool has_edge(size_t from, size_t to)
	{ return nodes_[from].edges_.end() != nodes_[from].edges_.find(to); }

	cost_t cost(size_t from, size_t to)
	{
		return nodes_[from].edges_[to].cost_;
	}

protected:
	nodes_t nodes_;

public:

	/*
		Testing helper stuff
	*/

#ifdef _DEBUG
	bool test_CheckColoring(coloring_result& cr) const
	{
		std::vector<int> node_colors = cr.nodes_colors();
		for ( nodes_t::const_iterator it  = nodes_.begin(); it != nodes_.end(); ++it)
			for (edges_t::const_iterator ie = it->edges_.begin(); ie != it->edges_.end(); ++ie)
				if (node_colors[ie->second.to()] == node_colors[ie->second.from()])
					throw std::exception("Incorrect coloring");
	}
#endif // #ifdef _DEBUG
};// digraph

/*
	Undirected graph class
*/

template
<
	  typename TNode   = int    // Some associated with node data
	, typename TLength = size_t // edge length type
>
class graph : public digraph<TNode, TLength>
{
typedef digraph<TNode, TLength> parent_t;
public:
	void connect(size_t from, size_t to, TLength cost = TLength())
	{
		parent_t::connect(from, to, cost);
		parent_t::connect(to, from, cost);
	}
};

/*
	Undirected multigraph class
*/

template
<
	  typename TNode   = int    // Some associated with node data
	, typename TLength = size_t // edge length type
>
class multigraph : public graph<TNode, TLength>
{

public:
	void connect(size_t from, size_t to, TLength cost = TLength())
	{
		parent_t::connect(from, to, parent_t::cost(from, to) + 1);
		parent_t::connect(to, from, parent_t::cost(to, from) + 1);
	}

	void contract(size_t from, size_t to)
	{
		for (node_t& node : nodes_) {
			
		}
	}
};

} // ff
#endif // ___FF_GRAPH__
