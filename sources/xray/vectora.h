////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VECTORA_H_INCLUDED
#define XRAY_VECTORA_H_INCLUDED

namespace xray {

#ifdef _STLP_USE_PTR_SPECIALIZATIONS
	template <typename T>
	struct qualified_type {
		typedef void*	result;
	}; // struct qualified_type
#else // #ifdef _STLP_USE_PTR_SPECIALIZATIONS
	template <typename T>
	struct qualified_type {
		typedef T		result;
	}; // struct qualified_type
#endif // #ifdef _STLP_USE_PTR_SPECIALIZATIONS

template < typename T >
class vectora_allocator;

namespace detail {

#define USER_ALLOCATOR	*( ( ( ::xray::vectora_allocator< typename qualified_type<T>::result >& )( *this ) ).m_allocator )
#include <xray/std_allocator.h>
#undef USER_ALLOCATOR

} // namespace detail

template < typename T >
class vectora_allocator :
	public detail::std_allocator< T >
{
public:
	template < typename T1 >	
	struct rebind			{
		typedef vectora_allocator< T1 >	other;
	};

	typedef ::xray::memory::base_allocator*	allocator_type;

	template < typename P >
	inline vectora_allocator	( vectora_allocator<P> const& allocator) :
		m_allocator		( allocator.m_allocator )
	{
	}

	inline vectora_allocator	( xray::memory::base_allocator* const allocator) :
		m_allocator		( allocator )
	{
	}

	inline vectora_allocator	( xray::memory::base_allocator& allocator) :
		m_allocator		( &allocator )
	{
	}

	allocator_type		m_allocator;

private:
	inline vectora_allocator	( ) : m_allocator	( NULL ) { }

	friend class ::std::vector< T, vectora_allocator< typename qualified_type<T>::result > >;
}; // struct allocator_type

template < typename T >
class vectora : public ::std::vector< T, vectora_allocator< typename qualified_type<T>::result > > {
public:
	typedef vectora< T >											self_type;
	typedef vectora_allocator< typename qualified_type<T>::result >	allocator_type;

private:
	typedef ::std::vector< T, allocator_type >						super;

public:
	typedef typename super::size_type								size_type;
	typedef typename super::value_type								value_type;

public:
	inline				vectora		( allocator_type allocator );
	inline				vectora		( xray::memory::base_allocator* allocator );
	inline				vectora		( allocator_type const& allocator, size_type count, value_type const& value );
	inline				vectora		( self_type const& other);
	template < typename input_iterator >
	inline				vectora		( allocator_type const& allocator, input_iterator const& first, input_iterator const& last );
	inline	self_type&	operator=	( self_type const& other );
	inline	u32			size		( ) const;

private:
	inline				vectora		( );
}; // class vector

} // namespace xray

#include <xray/vectora_inline.h>

#endif // #ifndef XRAY_VECTORA_H_INCLUDED