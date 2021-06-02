////////////////////////////////////////////////////////////////////////////
//	Module 		: std_allocator.h
//	Created 	: 24.06.2005
//  Modified 	: 14.04.2007
//	Author		: Dmitriy Iassenev
//	Description : memory allocator template class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MEMORY_ALLOCATOR_H_INCLUDED
#define CS_CORE_MEMORY_ALLOCATOR_H_INCLUDED

#include <typeinfo>

template <class T>
class cs_memory_allocator {
private:
	typedef cs_memory_allocator<T>	self_type;

public:
	typedef	size_t				size_type;
	typedef ptrdiff_t			difference_type;
	typedef T*					pointer;
	typedef T const*			const_pointer;
	typedef T&					reference;
	typedef T const&			const_reference;
	typedef T					value_type;

public:
	template<class _1>	
	struct rebind			{
		typedef cs_memory_allocator<_1> other;
	};

public:
								cs_memory_allocator	();
								cs_memory_allocator	(const cs_memory_allocator<T> &);

public:
	template<class _1>
	inline						cs_memory_allocator	(const cs_memory_allocator<_1>&);

public:
	template<class _1>
	inline	cs_memory_allocator<T>	&operator=			(const cs_memory_allocator<_1>&);

public:
	inline pointer				address				(reference value) const;
	inline const_pointer		address				(const_reference value) const;
	inline	pointer				allocate			(size_type n, void const*  p=0) const;
	inline	char*				__charalloc		(size_type n);
	inline	void				deallocate			(pointer p, size_type n) const;
	inline	void				deallocate			(void* p, size_type n) const;
	inline	void				construct			(pointer p, T const& value);
	inline	void				destroy				(pointer p);
	inline	size_type			max_size			() const;
};

template<class _0, class _1>
inline	bool operator==(const cs_memory_allocator<_0>&, const cs_memory_allocator<_1>&);
template<class _0, class _1>
inline	bool					operator!=			(const cs_memory_allocator<_0>&, const cs_memory_allocator<_1>&);

#include <cs/core/memory_allocator_inline.h>

#endif // #ifndef CS_CORE_MEMORY_ALLOCATOR_H_INCLUDED