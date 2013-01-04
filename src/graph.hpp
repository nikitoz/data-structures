#include <iostream>
#include "bheap.hpp"
#include <xutility>

namespace {
	template <typename T>
	struct ffgnode {
		ffgnode(T node, T* next) {
			data = node;
			next = next;
			valid = true;
			cost = 0;
			index = 0;
		}

		ffgnode(T node) {
			data = node;
			valid = true;
			next = NULL;
			cost = 0;
			index = 0;
		}

		ffgnode() { 
			valid = false;
			cost = 0;
			index = 0;
			next = NULL;
		}

		ffgnode(const ffgnode& prot, int cost) {
			data = prot.data;
			cost = cost;
			next = NULL;
			valid = true;
			index = prot.index;
		}

		int			cost;
		size_t		index;
		ffgnode*	next;

		T	 data;
		bool valid;
	};

	struct a_search_struct {
		size_t index;
		int cost;
		size_t pred;

		a_search_struct(size_t i, int c, size_t p) 
		{ index = i; cost = c; pred = p; }
		a_search_struct()
		{ index = 0; cost = 0; pred = 0; }
		void operator=(const a_search_struct& a1) 
		{ index = a1.index; cost = a1.cost; pred = a1.pred; }
		int getCost() const
		{ return cost; }
	};
//=========================================================================
//	ffgnode greater/less functors
//=========================================================================
	template <typename T>
	class greater_cost {
	public:
		bool operator()(const T& node1, const T& node2)
		{ return (node1.getCost() > node2.getCost()); }
	};

	template <typename T>
	class less_cost {
	public:
		bool operator()(const T& node1, const T& node2) const
		{ return (node1.getCost() < node2.getCost()); }
	};
}

struct a_search_out {
	size_t	 m_index;
	a_search_out* m_next;
	a_search_out(size_t index = -1, a_search_out* next = NULL)
	{ m_index = index; m_next = next; }
};
//=========================================================================
// ffgraph
//=========================================================================
template<typename TNode>
class ff_directed_graph {

typedef ffgnode<TNode>		node;

public:
	ff_directed_graph(size_t nodesCount) {
		nodes = new node[nodesCount];
		m_count = 0;
		m_size   = nodesCount;
	}

	void addNode(const TNode& data) {
		if (m_count == m_size) { // If we add to tyhe end, then resize array
			node* buf = nodes;
			nodes = new node[m_size*=2];
			std::copy(buf, buf + m_count, nodes);
			delete buf;
		}
			
		nodes[m_count] = node(data);
		nodes[m_count].index = m_count;
		m_count++;
	}

	node getNode(int identifier) {
		if (identifier >= m_count)
			return node();
		return nodes[identifier];
	}

	void addConnection(size_t fromId, size_t toId, int cost) {
		if ((fromId >= m_count) || (toId >= m_count) || (toId == fromId))
			return;

		node *newNode = new node(nodes[toId], cost);
		node *next = nodes[fromId].next;
		nodes[fromId].next = newNode; 
		newNode->next = next;
	}
	
	// return -1 if we don't have connection
	int getCost(size_t fromId, size_t toId) {
		if ((fromId >= m_count) || (toId >= m_count) || (toId == fromId))
			return -1;

		node* s = nodes+fromId;
		const TNode& lookFor = nodes[toId].constValue();

		while (s->next && s->next->data != lookFor) s = s->next;

		if (s->next)
			return s->next->cost;

		return -1;
	}

	// A* search
	a_search_out* fillShortestPath(int from, int to) const {
		if (! nodes[from].next ) 
			return NULL;

		ff_bheap<a_search_struct, less_cost<a_search_struct> > biHeap;

		size_t retValIndex = 0;

		node* current = nodes[from].next;
		int currentIndex = from;
		int additionalCost = 0;

		size_t *p = new size_t[m_count];
		memset(p, 0, sizeof(int)*m_count);

		// do while we don't get to our destination
		do {
			while (current != NULL) {
				if (p[current->index]==0) // if we didnt' find path to this, then add
					biHeap.insert(a_search_struct(current->index, current->cost + additionalCost, currentIndex));
				current = current->next;
			}

			a_search_struct pret;
			// retrieve best element, which is not found yet
			do {
				pret = biHeap.pop();
			} while (p[pret.index]);

			currentIndex = pret.index; // store index of our best element
		//	retValIndex = pret.pred;   // save previous element	
			p[pret.index] = pret.pred;	   // save cost to table
		//	retVal[retValIndex++] = currentIndex; // save this element to path
#if _DEBUG
			std::cout << "currIndex :" << currentIndex << " addco: " << additionalCost << std::endl;
#endif
			additionalCost = pret.cost;           // get cost to current element
			current = nodes[currentIndex].next; // get first neighbor of current element
		} while (currentIndex != to);

		//retVal[retValIndex] = -1;
		
		a_search_out *retVal = new a_search_out(to);
		a_search_out *newStruct(NULL);

		size_t id = to;
		while (id != from) {
			newStruct = new a_search_out(p[id], retVal);
			retVal = newStruct;
			id = p[id];
		}

		delete []p;
		return retVal;
	}

private:
	node* nodes;
	size_t m_count;
	size_t m_size;
};