#include "AVLTree_test.h"
#ifndef __FF_AVL_TREE__
#define __FF_AVL_TREE__

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <set>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <functional>
#include <cmath>
#include <memory.h>
#include <exception>


using namespace std;
namespace{
	//=========================================================================
	// Internal type for saving node data of the AVL-tree
	//=========================================================================
	template<typename TKey, typename TVal>
	struct anode {
		TKey key;
		TVal val;
		int balance;
		int childCount;

		anode<TKey, TVal> * left;
		anode<TKey, TVal> * right;
		anode():balance(0), right(NULL), left(NULL), childCount(0){}
		anode(TKey _k, TVal _v):balance(0), right(NULL), left(NULL), childCount(0){key = _k; val = _v;}
		anode(const anode<TKey, TVal>& a){(*this) = a;}

		const anode& operator=(const anode& _node){
			this->key = _node.key; this->val = _node.val; this->balance = _node.balance;
			return _node;
		}

		void calcBalance(){
			balance = ((left == NULL)? 0 : left->childCount+1) - ((right == NULL)? 0 : right->childCount+1);
		}

		void calcChildCount(){
			childCount = ((left == NULL)? 0 : left->childCount+1) + ((right == NULL)? 0 : right->childCount+1);
		}
	};
}
//=========================================================================
// Overloaded operators for anode type
//=========================================================================
template<typename TKey, typename TVal>
bool operator<(const TKey& a1, const TKey& a2){if (a1 < a2) return true; else return false;}
template<typename TKey, typename TVal>
bool operator>(const TKey& a1, const TKey& a2){if (a1 > a2) return true; else return false;}

//=========================================================================
// Map implementation based on AVL Tree
//=========================================================================
template <typename TKey, typename TVal, typename TPred = less<TKey> >
class ff_amap {
private:
	anode<TKey, TVal> *m_root;
	TPred m_predicate;

public:
	ff_amap(): m_root(NULL){}

	// Friend class for testing private member functions
	friend class FF_AVLTree_test;
	// Friend class for creating dot files
	friend class FF_AVL_GraphCreator;

	//=========================================================================
	void deleteNode(const TKey& key){
		anode<TKey, TVal>** nodeToRemove = NULL;
		stack< anode<TKey, TVal> ** > parents;
		if (m_root->key == key){
			nodeToRemove = &m_root; 
		} else{
			bool opResult(false);
			nodeToRemove = findNodePtrPtr(&m_root, parents, key, opResult);
		}
		if (nodeToRemove != NULL){
			anode<TKey, TVal>** substitution = NULL;
			anode<TKey, TVal>* substitutionChild = NULL;
			if ( ( substitution = findLeftMostChild(*nodeToRemove) ) == NULL){
				substitution = findRightMostChild(*nodeToRemove);
			}
			anode<TKey, TVal>* leftChi = NULL;
			anode<TKey, TVal>* rightChi = NULL;
			anode<TKey, TVal>* del = (*nodeToRemove);
			
			if (substitution != NULL){
				anode<TKey, TVal>* leftChi = NULL;
				anode<TKey, TVal>* rightChi = NULL;
				anode<TKey, TVal>* del = (*nodeToRemove);
				anode<TKey, TVal>* sub = (*substitution);

				leftChi = del->left!=sub? del->left : NULL;
				rightChi = del->right!=sub? del->right : NULL;

				delete del;                                

				(*nodeToRemove) = (sub);
				(*nodeToRemove)->left = leftChi;
				(*nodeToRemove)->right = rightChi;
				if(substitutionChild != NULL)
					substitution = &substitutionChild;
				else
					substitution = NULL;
				(*nodeToRemove)->calcChildCount();
				(*nodeToRemove)->calcBalance();
			}else{
				delete del;
				del = NULL;
				*nodeToRemove = NULL;
			}

			anode<TKey, TVal>** root = NULL;
			// Do rotations
			while (! parents.empty()) {
				root = parents.top();parents.pop();

				(*root)->calcChildCount();
				(*root)->calcBalance();

				if ((*root)->balance >= 2){
					anode<TKey, TVal>* left = (*root)->left;
					if(left->balance >= 0){
						littleRightRotate(root);
					} else if (left->balance <= -1){
						bigRightRotate(root);
					}
				} else if ((*root)->balance <= -2) {
					// Right tree outweightts left one
					anode<TKey, TVal>* right = (*root)->right;
					if (right->balance <= 0){
						//Simple left rotation is needed
						littleLeftRotate(root);
					} else if (right->balance  >= 1){
						//Double left rotation is needed because right-left outweights
						bigLeftRotate(root);
					}
				}
			}
			if (root && (*root)) m_root = *root;
		}
	}
	//=========================================================================
	const TVal& findNode(const TKey& key) /* throw (std::exception) */ {
		return operator[](key);
	}
	//=========================================================================
	const TVal* findPtrToNode(const TKey& key){
		if (m_root != NULL)
			return findNodeInt(m_root, key);
		return NULL;
	}
	//=========================================================================
	const TVal& operator[](const TKey& key)/* throw (std::exception) */ {
		if (m_root != NULL){
			const TVal* val = findNodeInt(m_root, key);
			if (val)
				return *val;
		}
		throw std::exception("ff_amap failed to find node!");
	}
	//=========================================================================
	void insertNode(const TKey& key, const TVal& val){
		if (m_root == NULL){
			m_root = new anode<TKey, TVal>(key, val);
		} else {
			anode<TKey, TVal>* freshNode = new anode<TKey, TVal>(key, val);
			insertInt(&m_root, freshNode);
		}
	}

private:

	//=========================================================================
	//a         b
	// \  =>   /
	//	b     a
	//=========================================================================
	void littleLeftRotate(anode<TKey, TVal>** pA){
		anode<TKey, TVal>* a = *pA;
		anode<TKey, TVal>* b = a->right;
		anode<TKey, TVal> *l = a->left, *c = b->left, *r = b->right;
		a->left = l;
		a->right = c;
		b->left = a;
		b->right = r;
		a->calcChildCount();
		b->calcChildCount();
		a->calcBalance();
		b->calcBalance();
		*pA = b;
	}
	//=========================================================================
	//  a    b
	// /  =>  \
	//b	       a
	//=========================================================================
	void littleRightRotate(anode<TKey, TVal>** pA){
		anode<TKey, TVal>* a = *pA;
		anode<TKey, TVal>* b = a->left;
		anode<TKey, TVal> *l = b->left, *c = b->right, *r = a->right;
		a->left = c;
		a->right = r;
		b->left = l;
		b->right = a;
		a->calcChildCount();
		b->calcChildCount();
		a->calcBalance();
		b->calcBalance();
		*pA = b;
	}
	//=========================================================================
	void bigLeftRotate(anode<TKey, TVal> **aa){
		anode<TKey, TVal> *a = *aa; 
		anode<TKey, TVal> *b = a->right;
		anode<TKey, TVal> *c = b->left;
		anode<TKey, TVal>** aaa = &b;
		littleRightRotate(aaa);
		a->right = *aaa;
		aaa = &a;
		littleLeftRotate(aaa);
		*aa = *aaa;
	}
	//=========================================================================
	void bigRightRotate(anode<TKey, TVal> **aa){
		anode<TKey, TVal> *a = *aa; 
		anode<TKey, TVal> *b = a->left;
		anode<TKey, TVal> *c = b->right;
		anode<TKey, TVal>** aaa = &b;
		littleLeftRotate(aaa);
		a->left = *aaa;
		aaa = &a;
		littleRightRotate(aaa);
		*aa = *aaa;
	}
	//=========================================================================
	anode<TKey, TVal>** findLeftMostChild(anode<TKey, TVal>* root){
		if (root != NULL && root->left != NULL && root->left->left != NULL){
			return findLeftMostChild(root->left);
		} else if (root == NULL){
			return NULL;
		} else if (root->left == NULL){
			return NULL;
		} else if (root->left->left == NULL){
			return &(root->left);
		}
		return NULL;
	}
	//=========================================================================
	anode<TKey, TVal>** findRightMostChild(anode<TKey, TVal>* root){
		if (root != NULL && root->right != NULL && root->right->right != NULL){
			return findRightMostChild(root->right);
		} else if (root == NULL){
			return NULL;
		} else if (root->right == NULL){
			return NULL;
		} else if (root->right->right == NULL){
			return &(root->right);
		}
		return NULL;
	}
	//=========================================================================
	anode<TKey, TVal>** findNodePtrPtr(anode<TKey, TVal>** pRoot, stack< anode<TKey, TVal> ** >& parents, const TKey& key, bool& opResult){
		parents.push(pRoot);
		anode <TKey, TVal> * root = *(pRoot);
		if (root->left != NULL && root->left->key == key){
			opResult = true;
			return &(root->left);
		} else if (root->right != NULL && root->right->key == key){
			opResult = true;
			return &(root->right);
		} else if (root->left != NULL && !m_predicate(root->key, key)){
			anode<TKey, TVal>** res = findNodePtrPtr(&(root->left), parents, key, opResult);
			if (opResult){
				return res;
			}
		} else if (root->right != NULL && m_predicate(root->key, key)){
			anode<TKey, TVal>** res = findNodePtrPtr(&(root->right), parents, key, opResult);
			if (opResult){
				return res;
			}
		}
		opResult = false;
		return NULL;
	}
	//=========================================================================
	const TVal* findNodeInt(const anode<TKey, TVal>* root, const TKey& key) const {
		if (root->key == key){
			return &(root->val);
		} else  {
			const TVal* retVal = NULL;
			if (root->left != NULL){
				retVal = findNodeInt(root->left, key);
				if (retVal)
					return retVal;
			}
			if (root->right != NULL){
				retVal = findNodeInt(root->right, key);
				if (retVal)
					return retVal;
			}
			return retVal;
		}
	}
	//=========================================================================
	void insertInt(anode<TKey, TVal>** pRoot, anode<TKey, TVal>* freshNode){
		anode<TKey, TVal>* root = *pRoot;
		if (root->key != freshNode->key){
			if(!m_predicate(root->key, freshNode->key)){
				//InsertTo left
				if(root->left == NULL){
					root->left = freshNode;
					root->childCount++;
					root->calcBalance();
				} else {
					insertInt((&root->left), freshNode);
				}

			} else {
				//InsertTo right
				if(root->right == NULL){
					root->right = freshNode;
					root->childCount++;
				} else {
					insertInt((&root->right), freshNode);
				}
			}

			root->calcChildCount();
			root->calcBalance();

			if (root->balance >= 2){
				anode<TKey, TVal>* left = root->left;
				if(left->balance >= 0){
					littleRightRotate(&root);
				} else if (left->balance <= -1){
					bigRightRotate(&root);
				}
			} else if (root->balance <= -2) {
				// Right tree outweightts left one
				anode<TKey, TVal>* right = root->right;
				if (right->balance <= 0){
					//Simple left rotation is needed
					littleLeftRotate(&root);
				} else if (right->balance  >= 1){
					//Double left rotation is needed because right-left outweights
					bigLeftRotate(&root);
				}
			}
			root->calcChildCount();
			root->calcBalance();
			*pRoot = root;
		}
	}
	//=========================================================================
	// Returns length of the max path in the tree. !Very inefficient! Only for tests
	//=========================================================================
	int getMaxPath(anode<TKey, TVal>* root){
		if (root != NULL){
			return std::max<int>(getMaxPath(root->left)+1, getMaxPath(root->right) + 1); 
		}else{ 
			return 0;
		}
	}
	//=========================================================================
	// Check balance factor of the tree, pass amount of elements
	//=========================================================================
	bool checkBalancing(int elements){
		int n = getMaxPath(m_root);
		int k = (long)ceil(1.44*(double)log10((double)elements) / (double)log10((double)2));
		if ((n - k) <= 2)
			return true;
		else  
			return false;
	}
};
// TODO: Write visualization and consistency check, check memory leaks, anode => template
// TODO: Write unit tests for big rotates
// TODO: Rewrite non-recursive insert
#endif