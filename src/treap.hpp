#ifndef __FF__TRIE__
#define __FF_TRIE__
namespace ff {
template
<
	  typename TTree
	, typename THeap
	, typename TTreePred = std::less<TTree>
	, typename THeapPred = std::less<THeap>
>
class treap
{
	struct node
	{
		TTree td_;
		THeap hd_;
		node* left_;
		node* right_;

		node(const TTree& td, const THeap& hd, node* left = 0, node* right = 0)
			: td_(td)
			, hd_(hd)
			, left_ (left)
			, right_(right)
		{ }
	} root_;

	TTreePred tree_pred_;
	THeapPred heap_pred_;

public:
	void insert(const TTree& td, const THeap& hd) {
		z_insert(&root_, td, hd);
	}

private:
	void z_insert(node* root, const TTree& td, const THeap& hd) {

	}

};     // treap
}      // ff
#endif // __FF_TRIE__