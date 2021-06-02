////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ALLOCATOR_HELPER_INLINE_H_INCLUDED
#define XRAY_ALLOCATOR_HELPER_INLINE_H_INCLUDED

#include <boost/type_traits/is_pod.hpp>

namespace xray {
namespace memory {

//-----------------------------------------------------------------------------------
// detail
//-----------------------------------------------------------------------------------
namespace detail {

template < typename T, bool >
struct get_top_pointer_helper 
{
	static inline pvoid call	( T*& pointer )
	{
		return					( pointer );
	}
}; // get_top_pointer_helper 

template < typename T >
struct get_top_pointer_helper<T, true>
{
	static inline pvoid call	( T*& pointer )
	{
		return					( dynamic_cast<pvoid>(pointer) );
	}
}; // get_top_pointer_helper 

struct call_destructor_predicate
{
	template <typename T>
	inline void operator ( )	( T* const pointer ) const
	{
		XRAY_UNREFERENCED_PARAMETER	( pointer );
		pointer->~T				( );
	}

}; // struct call_destructor_helper

template <class T, bool is_pod>
struct call_constructor_helper
{
	static inline void call			( T* const begin, T* const end )
	{
		T* i				= begin;
		for ( ; i != end; ++i )
			new ( i ) T		( );
	}

	static inline void call			( T* const value )
	{
		new ( value ) T		( );
	}
}; // struct call_constructor_helper

template <class T>
struct call_constructor_helper<T, true>
{
	static inline void call			( ... )
	{
	}
}; // struct call_constructor_helper

template <class T>
inline void call_constructor		( T* const begin, T* const end )
{
#ifdef _MSC_VER
	call_constructor_helper<T, __is_pod(T) >::call 	(begin, end);
#else // #ifdef _MSC_VER
	call_constructor_helper<T, boost::is_pod<T>::value >::call 	(begin, end);
#endif // #ifdef _MSC_VER
}

template <class T>
inline void call_constructor		( T* const value )
{
#ifdef _MSC_VER
	call_constructor_helper<T, __is_pod(T) >::call 	(value);
#else // #ifdef _MSC_VER
	call_constructor_helper<T, boost::is_pod<T>::value >::call 	(value);
#endif // #ifdef _MSC_VER
}

template < typename T >
inline pvoid get_top_pointer ( T*& pointer );

template <typename A, typename T, typename P>
inline void delete_array_helper_impl	( A& allocator, 
									  T*& pointer 
									  XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, 
									  P const& call_destructor_predicate )
{
	pvoid 	  real_pointer		= get_top_pointer<T>(pointer);
	u32*	  result			= ( ( u32* )real_pointer ) - 2;
	u32 const count				= result[0];
	ASSERT						( count < ( u32(1) << 31 ) );

	u32 const size				= result[1];

	pbyte i						= ( pbyte )pointer;
	pbyte e						= i + size*count;
	for ( ; i != e; i += size )
		call_destructor_predicate	( ( T* )i );

	allocator.free_impl			( result XRAY_CORE_DEBUG_PARAMETERS );
}

template <typename A, typename T, typename P>
void delete_array_helper_impl	(
		A& allocator, 
		T const*& const_pointer 
		XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, 
		P const& call_destructor_predicate
	)
{
	delete_array_helper_impl	( allocator, const_cast<T*&>( pointer ) XRAY_CORE_DEBUG_PARAMETERS, call_destructor_predicate );
}

template <typename A, typename T>
inline void free_helper_impl	( A& allocator, T*& pointer, pvoid const top_pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	allocator.free_impl			( top_pointer XRAY_CORE_DEBUG_PARAMETERS );
	pointer						= 0;
}

template < typename T >
inline pvoid get_top_pointer ( T*& pointer )
{
	return	get_top_pointer_helper<T,boost::is_polymorphic< T >::value >::call (pointer);
}

template <typename A, typename T, typename P>
inline void delete_helper_impl		( A& allocator, 
								  T*& pointer 
								  XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, 
								  P const& call_destructor_predicate )
{
	if ( !pointer )
		return;

	pvoid top_pointer			= detail::get_top_pointer( pointer );
	call_destructor_predicate	( pointer );
	detail::free_helper_impl	( allocator, pointer, top_pointer XRAY_CORE_DEBUG_PARAMETERS );
}

template <typename A, typename T, typename P>
void   delete_helper_impl		( A& allocator, 
								  T const*& const_pointer 
								  XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, 
								  P const& call_destructor_predicate )
{
	delete_helper_impl			( allocator, const_cast<T*&>( const_pointer ) XRAY_CORE_DEBUG_PARAMETERS, call_destructor_predicate );
}

} // namespace detail

//-----------------------------------------------------------------------------------
// allocations macro helpers
//-----------------------------------------------------------------------------------
#ifdef _MANAGED
#	pragma managed (push, on)
template < typename A, typename T >
inline void free_helper				( A& allocator, T* pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	if ( !pointer )
		return;

	detail::free_helper_impl	( allocator, pointer, pointer XRAY_CORE_DEBUG_PARAMETERS );
}
#	pragma managed( pop )
#else // #ifdef _MANAGED

template < typename A, typename T >
inline void free_helper				( A& allocator, T*& pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	if ( !pointer )
		return;

	detail::free_helper_impl	( allocator, pointer, (typename boost::remove_cv<T>::type *)pointer XRAY_CORE_DEBUG_PARAMETERS );
}
#endif // #ifdef _MANAGED

template <typename T>
template <typename A>
inline pvoid new_helper<T>::call		( A& allocator XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return						( allocator.malloc_impl( sizeof(T) 
													  XRAY_CORE_DEBUG_PARAMETERS_TYPEID(T) 
													  XRAY_CORE_DEBUG_PARAMETERS ) );
}

template <typename T>
template <typename A>
inline T* new_array_helper<T>::call	( A& allocator, u32 const count XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	u32*		result			= ( u32* )allocator.malloc_impl(
									2*sizeof( u32 ) + count*sizeof(T) 
									XRAY_CORE_DEBUG_PARAMETERS_TYPEID(T) XRAY_CORE_DEBUG_PARAMETERS );

	*result++					= count;
	*result++					= sizeof( T );
	T* const b					= ( T* )result;
	detail::call_constructor	( b, b + count );
	return						( b );
}

template < typename A, typename T >
inline void delete_array_helper	( A& allocator, T*& pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	detail::delete_array_helper_impl	( allocator, pointer XRAY_CORE_DEBUG_PARAMETERS, detail::call_destructor_predicate( ) );
}

template < typename A, typename T, typename P >
inline void delete_array_helper	( A& allocator, T*& pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, P const& delete_predicate)
{
	detail::delete_array_helper_impl	( allocator, pointer XRAY_CORE_DEBUG_PARAMETERS, delete_predicate );
}

#ifdef _MANAGED
#	pragma managed (push, on)
	template < typename A, typename T >
	inline void delete_helper	( A& allocator, T* pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
	{
		detail::delete_helper_impl		( allocator, pointer XRAY_CORE_DEBUG_PARAMETERS, detail::call_destructor_predicate( ) );
	}
	template < typename A, typename T, typename P >
	inline void delete_helper		( A& allocator, T* pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, P const& delete_predicate )
	{
		detail::delete_helper_impl		( allocator, pointer XRAY_CORE_DEBUG_PARAMETERS, delete_predicate );
	}

#	pragma managed( pop )
#else // #ifdef _MANAGED
	template < typename A, typename T >
	inline void delete_helper		( A& allocator, T*& pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
	{
		detail::delete_helper_impl	( allocator, pointer XRAY_CORE_DEBUG_PARAMETERS, detail::call_destructor_predicate() );
	}
	template < typename A, typename T, typename P >
	inline void delete_helper		( A& allocator, T*& pointer XRAY_CORE_DEBUG_PARAMETERS_DECLARATION, P const& delete_predicate )
	{
		detail::delete_helper_impl	( allocator, pointer XRAY_CORE_DEBUG_PARAMETERS, delete_predicate );
	}
#endif // #ifdef _MANAGED

template <typename A>
inline pvoid   malloc_helper			( A& allocator, size_t const size, pcstr description XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return				allocator.malloc_impl( size, description XRAY_CORE_DEBUG_PARAMETERS );
}

template <typename A>
inline pvoid   malloc_helper			( A& allocator, size_t const size XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return				allocator.malloc_impl( size XRAY_CORE_DEBUG_PARAMETERS );
}

template <typename A, typename T>
inline pvoid   realloc_helper			( A& allocator, T* pointer, size_t const size, pcstr description XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return				allocator.realloc_impl( pointer, size, description XRAY_CORE_DEBUG_PARAMETERS);
}

template <typename A, typename T>
inline pvoid   realloc_helper			( A& allocator, T* pointer, size_t const size XRAY_CORE_DEBUG_PARAMETERS_DECLARATION )
{
	return				allocator.realloc_impl( pointer, size XRAY_CORE_DEBUG_PARAMETERS);
}

} // namespace memory
} // namespace xray

#endif // #ifndef XRAY_ALLOCATOR_HELPER_INLINE_H_INCLUDED