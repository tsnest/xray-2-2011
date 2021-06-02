////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_allocator_inline.h
//	Created 	: 18.03.2007
//  Modified 	: 14.04.2007
//	Author		: Dmitriy Iassenev
//	Description : memory allocator template class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MEMORY_ALLOCATOR_INLINE_H_INCLUDED
#define CS_CORE_MEMORY_ALLOCATOR_INLINE_H_INCLUDED

#define TEMPLATE_SPECIALIZATION	template <class T>
#define MEMORY_ALLOCATOR		cs_memory_allocator<T>

TEMPLATE_SPECIALIZATION
MEMORY_ALLOCATOR::cs_memory_allocator									()
{
}

TEMPLATE_SPECIALIZATION
MEMORY_ALLOCATOR::cs_memory_allocator									(self_type const& )
{
}

TEMPLATE_SPECIALIZATION
template<class other>
MEMORY_ALLOCATOR::cs_memory_allocator									(const cs_memory_allocator<other> &)
{
}

TEMPLATE_SPECIALIZATION
template<class other>
MEMORY_ALLOCATOR& MEMORY_ALLOCATOR::operator=							(const cs_memory_allocator<other> &)
{
	return		*this;
}

TEMPLATE_SPECIALIZATION
typename MEMORY_ALLOCATOR::pointer MEMORY_ALLOCATOR::address			(reference value) const
{
	return		&value;
}

TEMPLATE_SPECIALIZATION
typename MEMORY_ALLOCATOR::const_pointer MEMORY_ALLOCATOR::address		(const_reference value) const
{
	return		&value;
}

TEMPLATE_SPECIALIZATION
typename MEMORY_ALLOCATOR::pointer MEMORY_ALLOCATOR::allocate			(size_type n, void const* p) const
{
	return		(pointer)cs_realloc( p,::std::max(n,size_type(1))*sizeof(T), typeid(T).name() ) ;
}

TEMPLATE_SPECIALIZATION
char* MEMORY_ALLOCATOR::__charalloc									(size_type n)
{
	return 		(char _FARQ* )allocate(n);
}

TEMPLATE_SPECIALIZATION
void MEMORY_ALLOCATOR::deallocate										(pointer p, size_type n) const
{
	CS_UNREFERENCED_PARAMETER	( n );
	cs_realloc	( p, 0, typeid(T).name() );
}

TEMPLATE_SPECIALIZATION
void MEMORY_ALLOCATOR::deallocate										(void* p, size_type n) const
{
	allocator	(p,0);
}

TEMPLATE_SPECIALIZATION
void MEMORY_ALLOCATOR::construct										(pointer p, T const&  value)
{
	new(p)		T(value);
}

TEMPLATE_SPECIALIZATION
void MEMORY_ALLOCATOR::destroy											(pointer p)
{
	p->~T		();
}

TEMPLATE_SPECIALIZATION
typename MEMORY_ALLOCATOR::size_type MEMORY_ALLOCATOR::max_size		() const
{
	size_type	count = ((size_type)(-1))/sizeof(T);
	if (count)
		return	count;

	return		1;
}

#undef TEMPLATE_SPECIALIZATION
#undef MEMORY_ALLOCATOR

namespace luabind {

template<class _0, class _1>
inline bool operator==	(const cs_memory_allocator<_0> &, const cs_memory_allocator<_1> &)
{
	return 		true;
}

template<class _0, class _1>
inline bool operator!=	(const cs_memory_allocator<_0> &, const cs_memory_allocator<_1> &)
{
	return 		false;
}

}

#endif // #ifndef CS_CORE_MEMORY_ALLOCATOR_INLINE_H_INCLUDED