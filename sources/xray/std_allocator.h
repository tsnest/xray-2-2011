////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#ifndef USER_ALLOCATOR
#	error please define USER_ALLOCATOR macro before including this file
#endif // #ifndef USER_ALLOCATOR

template < typename T >
class std_allocator {
private:
	typedef std_allocator<T>	self_type;

public:
	typedef	size_t				size_type;
	typedef ptrdiff_t			difference_type;
	typedef T*					pointer;
	typedef const T*			const_pointer;
	typedef T&					reference;
	typedef const T&			const_reference;
	typedef T					value_type;

public:
	template<class _1>	
	struct rebind			{
		typedef std_allocator<_1> other;
	};

public:
								std_allocator		();
								std_allocator		(std_allocator<T> const&);

public:
	template<class _1>
	inline						std_allocator		(std_allocator<_1> const&);

public:
	template<class _1>
	inline	std_allocator<T>&	operator=			(std_allocator<_1> const&);

public:
	inline	pointer				address				(reference value) const;
	inline	const_pointer		address				(const_reference value) const;
	inline	pointer				allocate			(size_type n, void* p = 0) const;
	inline	pointer				allocate			(size_type n, size_type& allocated) const;
	inline	pointer				_M_allocate			(size_type n, void* p = 0) const;
	inline	pointer				_M_allocate			(size_type n, size_type& allocated) const;
	inline	char*				__charalloc			(size_type n);
	inline	void				deallocate			(pointer p, size_type n) const;
	inline	void				deallocate			(void* p, size_type n) const;
	inline	void				construct			(pointer p, T const& value);
	inline	void				destroy				(pointer p);
	inline	size_type			max_size			() const;
};

//#if 1
template < typename left, typename right >
inline	bool					operator==			(std_allocator<left> const&, std_allocator<right> const&)
{
	return true;
}

template < typename left, typename right >
inline	bool					operator!=			(std_allocator<left> const&, std_allocator<right> const&)
{
	return false;
}
//#endif // #if 0

#include <xray/std_allocator_inline.h>