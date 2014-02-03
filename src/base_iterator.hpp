#ifndef __FF__BASE__ITERATOR__
#define __FF__BASE__ITERATOR__

namespace ff
{
template <typename TNode, typename TData>
struct base_iterator {
	typedef TNode node_t;
	typedef TData data_t;
};
}  //ff
#endif //__FF__BASE__ITERATOR__