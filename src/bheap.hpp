#ifndef __FF_BIHEAP__
#define __FF_BIHEAP__

#include <cstdlib>
#include <xutility>
#include <functional>

#ifdef _MSC_VER
	#pragma warning(disable: 4996)
#endif

namespace ff
{

/*
	Simple binary heap implementation [C++98]
*/

template <typename T, typename Pred = std::greater<T> >
class bheap
{
public:
	T pop()
	{
		if (m_capacity*3/4 > m_size)
			decreaseFourth();

		T ret; // do deep copy
		if (m_size) {
			ret = m_d[0];
			m_d[0] = m_d[--m_size];
			heapify(0);
		}
		return ret;
	}

	void insert(const T& newValue)
	{
		if (m_size == m_capacity)
			increaseTwice();

		m_d[m_size] = newValue;
		m_size++;
		for (size_t i = parent(m_size-1); i != UNDER_ZERO; --i)
			heapify(i);
	}

	bool empty() const
	{
		return m_size == 0;
	}

	const T* getArray() const
	{ return m_d; }

	size_t getSize() const
	{ return m_size; }

	const T& high() const
	{ return m_d[0]; }

	void setData(T* data, size_t size)
	{
		if (size > m_capacity) {
			delete[] m_d;
			m_d = new T[size];
			m_capacity = m_size = size;
		}
		m_size = size;
		std::copy(data, data + m_size, m_d);
		for (size_t i = 0; i < m_size/2; ++i)
			heapify(i);
	}

	/*
	    Constructors/destructor operators
    */

	bheap(int size)
	{
		m_d = NULL;
		m_d = new T[size];
		m_capacity = m_size = size;
	}

	bheap()
	{
		m_d = NULL;
		m_d = new T[2];
		m_capacity = 2;
		m_size = 0;
	}

	bheap(const bheap& _heap)
	{
		m_capacity = m_size = _heap.getSize();
		m_d = new T[m_size];
		std::copy(_heap.m_d, _heap.m_d + _heap.m_size, m_d);
		m_predicate = _heap.m_predicate;
	}

	const bheap& operator=(const bheap& _heap)
	{
		if (m_capacity < _heap.m_size) {
			m_capacity = _heap.getSize();
			delete []m_d;
			m_d = new T[m_size];
		}
		m_size = _heap.getSize();
		std::copy(_heap.m_d, _heap.m_d + _heap.m_size, m_d);
		m_predicate = _heap.m_predicate;
	}

	virtual ~bheap()
	{
		delete[] m_d;
		m_d = NULL;
	}

private:

	void increaseTwice()
	{ changeSize(m_capacity*2); }

	void decreaseFourth()
	{ changeSize(m_capacity*3/4);	}

	size_t parent(size_t meow) const
	{ return meow/2 + 1; }

	size_t leftChild(size_t me) const
	{ return me*2 + 1; }

	size_t rightChild(size_t me) const
	{ return me*2+2; }

	void heapify(size_t ind)
	{
		size_t violationIndex = getViolationChild(ind);
		if ( violationIndex != ind ) {
			T tmp = m_d[ind];
			m_d[ind] = m_d[violationIndex];
			m_d[violationIndex] = tmp;
			heapify(violationIndex);
		}
	}
	
	void changeSize(size_t newSize)
	{
		T* tmp = m_d;
		m_d = new T[newSize];
		m_capacity = newSize;
		std::copy(tmp, tmp + m_size, m_d);
		delete[] tmp;
	}

	size_t getViolationChild(size_t ind) const
	{
		size_t violationIndex = ind;
		size_t leftChildIndex = leftChild(ind);
		size_t rightChildIndex = rightChild(ind);

		if (leftChildIndex < m_size && m_predicate(m_d[leftChildIndex], m_d[ind])) 
			violationIndex = leftChildIndex;
		if (rightChildIndex < m_size && m_predicate(m_d[rightChildIndex], m_d[violationIndex]))
			violationIndex = rightChildIndex;

		return violationIndex;
	}

	T* m_d;
	size_t m_capacity;
	size_t m_size;
	Pred m_predicate;

#ifdef _DEBUG
public:
	bool checkHeapProperty() const
	{
		for(size_t i = 0; i < m_size/2; ++i)
			if (i != getViolationChild(i))
				return false;

		return true;
	}
#endif //_DEBUG

	static const size_t UNDER_ZERO = (size_t)(0) - size_t(1);

}; //bheap
}  //ff
#endif //__FF_BIHEAP__