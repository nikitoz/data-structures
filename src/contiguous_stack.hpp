#ifndef __FF__CONT_STACK__
#define __FF__CONT_STACK__
#include <memory>
/*
	Stack of contiguous elements, designed for speed
	Check for underflow/overflow by yourself
*/
template <typename T, typename alloc_t = std::allocator<T>>
class contiguous_stack {
public:
	explicit contiguous_stack(size_t n) : d_(nullptr), n_(n), head_(UNDER_ZERO) {
		d_ = allocator_.allocate(n, 0);
	}

	~contiguous_stack() {
		allocator_.deallocate(d_, n_);
	}

	inline void push(const T& d) {
		d_[++head_]  = d;
	}

	inline const T& top() {
		return d_[head_];
	}

	inline const T& pop() {
		return d_[head_--];
	}

	inline bool empty() const {
		return UNDER_ZERO == head_;
	}

	inline bool full() const {
		return head_ == n_ - 1;
	}

private:
	T*      d_;
	size_t  n_;
	size_t  head_;
	alloc_t allocator_;
	static const size_t UNDER_ZERO = size_t(0-1);
};

#endif // __FF__CONT_STACK__
