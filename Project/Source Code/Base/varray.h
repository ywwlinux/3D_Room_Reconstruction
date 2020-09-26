#if !defined(__ARRAY_H_INCLUDED__)
#define __ARRAY_H_INCLUDED__

#include "assert.h"

#pragma warning(disable: 4018 4267 )

namespace base {

template<class _T >
class varray {
public:
	typedef _T* iterator;
	typedef const _T* const_iterator;

	explicit varray() : 
		m_buf(0), m_sz(0), m_alloc(0) {}
	explicit varray(size_t sz)  : 
		m_buf(0), m_sz(0), m_alloc(0) {
			resize(sz);
		}
	explicit varray(size_t sz, const _T& v) : 
		m_buf(0), m_sz(0), m_alloc(0) {
		resize(sz, v);
	}
	varray(const varray<_T>& a) : 
		m_buf(0), m_sz(0), m_alloc(0) {
		*this = a;
	}

	~varray() {
		clear();
	}

	void clear() {
		if (m_buf) delete [] m_buf;
		m_buf = 0;
		m_sz  = 0;
		m_alloc = 0;
	}

	void resize(size_t n) {
		if (m_alloc < n) reserve(n);
		m_sz = n;
	}

	void resize(size_t n, const _T& v) {
		size_t pre_sz = size();
		resize(n);
		for (int i = pre_sz; i < size(); i++) at(i) = v;
	}

	void reserve(size_t n) {
		if (n <= m_alloc) return;
		
		_T* b = new _T[n];
		if (m_buf) {
			for (unsigned int i = 0; i < m_sz; i++) b[i] = m_buf[i];
			delete [] m_buf;
		}
		m_buf = b;
		m_alloc = n;
	}
	void push_back(const _T& v) {
		if (m_alloc < m_sz+1) {
			if (m_sz == 0) reserve(2);
			else reserve(m_sz * 2);
		}

		//resize(m_sz+1);
		m_buf[m_sz] = v;
		m_sz++;
	}
	void insert(iterator p, const _T& v) {
		_T bk = m_buf[m_sz-1];
		for (iterator i = end()-1; i != p; i--) *i = *(i-1);
		*p = v;
		resize(m_sz+1);
		back() = bk;
	}
	void insert(iterator p, const_iterator first, const_iterator last) {
		varray<_T> buf(m_sz + last - first);
		const_iterator i;
		iterator j = buf.begin();
		for (i = begin(); i != p; i++, j++) *j = *i;
		for (i = first; i != last; i++, j++) *j = *i;
		for (i = p; i != end(); i++, j++) *j = *i;
		*this = buf;
	}

	void pop_back() {
		if (m_sz > 0) erase(end()-1);
	}
	void pop_front() {
		if (m_sz > 0) erase(begin(), begin()+1);
	}
	void erase(iterator p) {
		if (m_sz <= 0) return;
		for (iterator i = p+1; i != end(); i++) {
			*(i-1) = *i;
		}
		m_sz--;
	}
	void erase(iterator from, iterator to) {
		iterator i;
		iterator ii = from;
		for (i = to; i != end(); i++, ii++) {
			*ii = *i;
		}
		m_sz-=to-from;
	}


	_T& front() {return m_buf[0];}
	_T& back()  {return m_buf[m_sz-1];}
	const _T& front() const {return m_buf[0];}
	const _T& back()  const {return m_buf[m_sz-1];}
	
#if defined(_DEBUG)
	_T&  at(size_t idx) {assert(idx<size());return m_buf[idx];}
	const _T&  at(size_t idx) const {assert(idx<size());return m_buf[idx];}
	_T& operator[](size_t idx) {assert(idx<size());return m_buf[idx];}
	const _T& operator[](size_t idx) const {assert(idx<size());return m_buf[idx];}
#else
	_T&  at(size_t idx) {return m_buf[idx];}
	const _T&  at(size_t idx) const {return m_buf[idx];}
	_T& operator[](int idx) {return m_buf[idx];}
	const _T& operator[](int idx) const {return m_buf[idx];}
#endif

	iterator begin() {return m_buf;}
	iterator end() {return &m_buf[m_sz];}
	const iterator begin() const {return m_buf;}
	const iterator end()  const {return &m_buf[m_sz];}
	//size_t size() const {return m_sz;}
	//size_t capacity() const {return m_alloc;}
	int size() const {return m_sz;}
	int capacity() const {return m_alloc;}
	bool empty() const { if( m_sz==0) return true; else return false;}

	varray& operator=(const varray& a) {
		if (a.size() == 0) {
			clear();
		} else {
			iterator i, j;
			resize(a.size());
			for (i=a.begin(),j=begin(); i!=a.end(); i++,j++) *j = *i;
		}
		return *this;
	}

protected:

protected:
	_T* m_buf;
	//size_t m_sz;
	//size_t m_alloc;

	int m_sz;
	int m_alloc;
};
} // namespace base

using namespace base;
#endif
