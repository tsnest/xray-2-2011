////////////////////////////////////////////////////////////////////////////
//	Created		: 18.10.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////


#ifndef XRAY_RENDER_ENGINE_POLYMORPH_VECTOR_INLINE_H_INCLUDED
#define XRAY_RENDER_ENGINE_POLYMORPH_VECTOR_INLINE_H_INCLUDED


namespace xray {
namespace render {


// template <typename T, typename base>
// inline polymorph_vector<T,base>::polymorph_vector	( pvoid const buffer, size_type const max_count, size_type const live_count ) :
// 	_begin()			( (T*)buffer),
// 	_end()			( ((T*)buffer) + live_count )
// #ifdef DEBUG
// 	,_max_end()		( (T*)buffer + max_count)
// #endif // #ifdef DEBUG
// {
// 	XRAY_UNREFERENCED_PARAMETER	( max_count );
// 	R_ASSERT		( live_count <= max_count );
// }
// 
// template <typename T, typename base>
// inline polymorph_vector<T,base>::polymorph_vector	( pvoid const buffer, size_type const max_count, size_type const count, value_type const& value ) :
// 	_begin()			( ( T* )buffer ),
// 	_end()			( ( T* )buffer )
// #ifdef DEBUG
// 	,_max_end()		( ( T* )buffer + max_count )
// #endif // #ifdef DEBUG
// {
// 	XRAY_UNREFERENCED_PARAMETER	( max_count );
// 
// 	assign			( count, value );
// }
// 
// template <typename T, typename base>
// inline polymorph_vector<T,base>::polymorph_vector	( pvoid const buffer, size_type const max_count, self_type const& other ) :
// 	_begin()			( ( T* )buffer ),
// 	_end()			( ( T* )buffer )
// #ifdef DEBUG
// 	,_max_end()		( ( T* )buffer + max_count )
// #endif // #ifdef DEBUG
// {
// #ifndef DEBUG
// 	XRAY_UNREFERENCED_PARAMETER	( max_count );
// #endif // #ifndef DEBUG
// 
// 	assign			( other.begin( ), other.end( ) );
// }
// 
// template <typename T, typename base>
// template < typename input_iterator >
// inline polymorph_vector<T,base>::polymorph_vector	( pvoid const buffer, size_type const max_count, input_iterator const& begin, input_iterator const& end ) :
// 	_begin()			( ( T* )buffer ),
// 	_end()			( ( T* )buffer )
// #ifdef DEBUG
// 	,_max_end()		( ( T* )buffer + max_count )
// #endif // #ifdef DEBUG
// {
// #ifndef DEBUG
// 	XRAY_UNREFERENCED_PARAMETER	( max_count );
// #endif // #ifndef DEBUG
// 
// 	assign			( begin, end );
// }

template <typename T, typename base>
inline polymorph_vector<T,base>::polymorph_vector	( u32 capacity): polymorph_vector_base<base>( sizeof(T), boost::is_pointer<T>::value)
{
	create_buffer( capacity);
}

template <typename T, typename base>
inline polymorph_vector<T,base>::polymorph_vector	( ):
polymorph_vector_base<base>( sizeof(T), boost::is_pointer<T>::value)
{
	set_buffer( (pointer)NULL, (pointer)NULL);
}

template <typename T, typename base>
inline polymorph_vector<T,base>::~polymorph_vector( )
{
	clear			( );

	char* Tptr =	(char*)_begin();
	DELETE_ARRAY	(Tptr);
}

template <typename T, typename base>
inline	void polymorph_vector<T,base>::create_buffer		( u32 capacity)
{
	pointer const	begin		= pointer(NEW_ARRAY (char, capacity*sizeof(T)));
#ifdef DEBUG
	pointer const	max_end		= ( begin + capacity);
#else
	pointer const	max_end		= 0;
#endif // #ifdef DEBUG

	set_buffer( begin, max_end);
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::self_type &polymorph_vector<T,base>::operator=	( self_type const& other )
{
	assign			( other.begin( ), other.end( ) );
	return			( *this );
}

template <typename T, typename base>
template < typename input_iterator >
inline void polymorph_vector<T,base>::assign	( input_iterator begin, input_iterator const& end )
{
	destroy			( _begin(), _end() );

	set_end			( _begin() + ( end - begin ));
	ASSERT			( _max_end() >= _end() );

	for ( iterator I = _begin(); begin != end; ++begin, ++I )
		construct	( I, *begin );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::assign	( size_type const count, non_volatile_const_reference value )
{
	destroy			( _begin(), _end() );

	set_end		( _begin() + count);
	ASSERT		( _max_end() >= _end() );

	for ( iterator I = _begin(); I != _end(); ++I )
		construct	( I, value );
}

// template <typename T, typename base>
// inline void polymorph_vector<T,base>::swap		( self_type& other )
// {
// 	COMPILE_ASSERT	( false, do_not_use_this_function_for_fixed_and_buffer_vectors );
// 
// #if 0
// 	std::swap		( _begin(),		other._begin() );
// 	std::swap		( _end(),		other._end() );
// #	ifdef DEBUG
// 		std::swap	( _max_end(),	other._max_end() );
// #	endif // #ifdef DEBUG
// #endif // #if 0
// }

template <typename T, typename base>
inline void polymorph_vector<T,base>::clear	( )
{
	destroy			( (T*)_begin(), (T*)_end() );
	m_end			= m_begin;
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::resize	( size_type const size )
{
	size_type		current_size = this->size( );
	if ( size == current_size )
		return;

	if ( size < current_size ) {
		destroy		( _begin() + size, _begin() + current_size );
		set_end		( _begin() + size);
		ASSERT		( _max_end() >= _end() );
		return;
	}

	ASSERT			( size > current_size );
	construct		( _begin() + current_size, _begin() + size );
	set_end			( _begin() + size);
	ASSERT			( _max_end() >= _end() );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::resize	( size_type const size, non_volatile_const_reference value )
{
	size_type		current_size = this->size( );
	if ( size == current_size )
		return;

	if ( size < current_size ) {
		destroy		( _begin() + size, _begin() + current_size );
		set_end		( _begin() + size);
		ASSERT	( _max_end() >= _end() );
		return;
	}

	ASSERT			( size > current_size );
	construct		( _begin() + current_size, _begin() + size, value );
	set_end			( _begin() + size);
	ASSERT			( _max_end() >= _end() );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::reserve	( size_type const size )
{
	XRAY_UNREFERENCED_PARAMETER	(size);
	// nothing to do here
}

template <typename T, typename base>
template < typename input_iterator >
inline void polymorph_vector<T,base>::insert	( iterator const &where, input_iterator begin, input_iterator const& end )
{
	ASSERT			( where >= _begin() );

	difference_type	count = end - begin;
	ASSERT		( _end() + count <= _max_end() );
	
	iterator		j = _end() + count - 1;		// new end
	iterator		i = _end() - 1;				// old end
	iterator		e = where - 1;
	for ( ; i != e; --i, --j ) {
		construct	( j, *i );
		destroy		( i );
	}

	_end()			+= count;

	for ( iterator i = where, e = i + count; i != e; ++i, ++begin )
		construct	( i, *begin );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::insert	( iterator const& where, size_type const count, non_volatile_const_reference value )
{
	ASSERT			( where >= _begin() );
	ASSERT		( _end() + count <= _max_end() );

	iterator		j = _end() + count - 1;		// new end
	iterator		i = _end() - 1;				// old end
	iterator		e = where - 1;
	for ( ; i != e; --i, --j ) {
		construct	( j, *i );
		destroy		( i );
	}

	set_end			( _end() + count);

	for ( iterator i = where, e = i + count; i != e; ++i )
		construct	( i, value );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::insert	( iterator const &where, non_volatile_const_reference value )
{
	insert			( where, 1, value );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::erase	( iterator const& begin, iterator const& end )
{
	ASSERT			( _begin() <= begin );
	ASSERT			( _end() >= begin );
	
	ASSERT			( _begin() <= end );
	ASSERT			( _end() >= end );
	
	ASSERT			( begin <= end );
	if ( begin == end )
		return;

	for ( iterator i = begin, j = end; j != _end(); ++i, ++j ) {
		destroy		( i );
		construct	( i, *j );
	}

	size_type		count = size_type(end - begin);
	size_type		size = this->size( );
	ASSERT			( size >= count );
	size_type		new_size = size - count;
	destroy			( _begin() + new_size, _end() );
	set_end			( _begin() + new_size);
	ASSERT			( _max_end() >= _end() );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::erase	( iterator const& where )
{
	erase			( where, where + 1 );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::pop_back	( )
{
	ASSERT			( !empty( ) );
	set_end			( _end() - 1);
	destroy			( _end() );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::push_back( non_volatile_const_reference value )
{
	ASSERT		( _end() < _max_end() );
	construct	( _end(), value );
	set_end		( _end() + 1);
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::reference polymorph_vector<T,base>::at					( size_type const index )
{
	ASSERT			( index < size( ) );
	return			( _begin()[ index ] );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_reference polymorph_vector<T,base>::at			( size_type const index ) const
{
	ASSERT			( index < size( ) );
	return			( _begin()[ index ] );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::reference polymorph_vector<T,base>::operator [ ]		( size_type const index )
{
	ASSERT			( index < size( ) );
	return			( _begin()[ index ] );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_reference polymorph_vector<T,base>::operator[]	( size_type const index ) const
{
	ASSERT			( index < size( ) );
	return			( _begin()[ index ] );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::reference polymorph_vector<T,base>::back				( )
{
	ASSERT			( !empty( ) );
	return			( *( _end() - 1 ) );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_reference polymorph_vector<T,base>::back			( ) const
{
	ASSERT			( !empty( ) );
	return			( *( _end() - 1 ) );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::reference polymorph_vector<T,base>::front				( )
{
	ASSERT			( !empty( ) );
	return			( *_begin() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_reference polymorph_vector<T,base>::front			( ) const
{
	ASSERT			(!empty( ));
	return			( *_begin() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::iterator polymorph_vector<T,base>::begin				( )
{
	return			( _begin() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_iterator polymorph_vector<T,base>::begin			( ) const
{
	return			( _begin() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::iterator polymorph_vector<T,base>::end					( )
{
	return			( _end() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_iterator polymorph_vector<T,base>::end			( ) const
{
	return			( _end() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::reverse_iterator polymorph_vector<T,base>::rbegin		( )
{
	return			( reverse_iterator( end( ) ) );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_reverse_iterator polymorph_vector<T,base>::rbegin	( ) const
{
	return			( const_reverse_iterator( end( ) ) );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::reverse_iterator polymorph_vector<T,base>::rend			( )
{
	return			( reverse_iterator( begin( ) ) );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::const_reverse_iterator polymorph_vector<T,base>::rend	( ) const
{
	return			( const_reverse_iterator( begin( ) ) );
}

template <typename T, typename base>
inline bool polymorph_vector<T,base>::empty											( ) const
{
	return			( _begin() == _end() );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::size_type polymorph_vector<T,base>::size				( ) const
{
	return			( size_type(_end() - _begin()) );
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::size_type polymorph_vector<T,base>::capacity			( ) const
{
#ifdef DEBUG
	return			( size_type(_max_end() - _begin()) );
#else // #ifdef DEBUG
	return			( size_type(_end() - _begin()) );
#endif // #ifdef DEBUG
}

template <typename T, typename base>
inline typename polymorph_vector<T,base>::size_type polymorph_vector<T,base>::max_size			( ) const
{
#ifdef DEBUG
	return			( size_type(_max_end() - _begin()) );
#else // #ifdef DEBUG
	return			( size_type(_end() - _begin()) );
#endif // #ifdef DEBUG
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::construct( pointer p)
{
	memory::detail::call_constructor( (non_volatile_pointer)p, (non_volatile_pointer)p+1 );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::construct( pointer p, non_volatile_const_reference value )
{
	new ( (non_volatile_pointer)p ) T		( value );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::construct( iterator begin, iterator const& end )
{
	for ( ; begin != end; ++begin )
		construct	( (non_volatile_pointer)begin );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::construct( iterator begin, iterator const& end, non_volatile_const_reference value )
{
	for ( ; begin != end; ++begin )
		construct	( begin, value );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::destroy	( pointer const p )
{
	XRAY_UNREFERENCED_PARAMETER	( p );
	p->~T			( );
}

template <typename T, typename base>
inline void polymorph_vector<T,base>::destroy	( iterator begin, iterator const& end )
{
	for ( ; begin != end; ++begin )
		destroy		( (non_volatile_pointer)begin );
}

template <typename T, typename base>
inline void swap					( polymorph_vector<T,base>& left, polymorph_vector<T,base>& right )
{
	left.swap		( right );
}

//template <typename T, typename base> 
//inline	typename polymorph_vector<T,base>::pointer		polymorph_vector<T,base>::__begin_impl()
//{
//
//}
//
//template <typename T, typename base> 
//inline	typename polymorph_vector<T*,base>::pointer		polymorph_vector<T,base>::__begin_impl()
//{
//
//}

// template <>
// template <typename T, typename base>
// inline typename polymorph_vector<T,base>::pointer polymorph_vector<T,base>::__begin_impl<false>	( )
// {
// 	return (pointer)m_begin;
// }


// template<>				
// template <typename T, typename base>
// void		polymorph_vector<T,base>::set_buffer<true>	( pointer begin, pointer max_end)	
// { 
// 	m_begin_ptr	= begin; 
// 	m_end_ptr	= m_begin_ptr;
// 	m_max_end_ptr	= max_end;
// }
// 
// template<>				
// template <typename T, typename base>
// void		set_buffer<false>	( pointer begin, pointer max_end);//	{ m_begin		= begin; m_end		= m_begin, m_max_end			= max_end;}



//////////////////////////////////////////////////////////////////////////

template <typename T>
inline typename T* polymorph_vector_base<T>::operator [ ]		( size_type const index )
{
	ASSERT			( m_begin + index * m_pitch < m_end  );
	return			( m_begin + index * m_pitch );
}

template <typename T>
inline typename T const * polymorph_vector_base<T>::operator[]			( size_type const index ) const
{
	ASSERT			( m_begin + index * m_pitch < m_end  );
	return			( m_begin + index * m_pitch );
}

template <typename T>
inline  typename polymorph_vector_base<T>::size_type polymorph_vector_base<T>::size()
{
//.	return (m_end - m_begin)/m_pitch;
	return size_type(m_end - m_begin);
}

template <typename T>
inline	typename polymorph_vector_base<T>::iterator		polymorph_vector_base<T>::begin ( ) 
{
	typename polymorph_vector_base<T>::iterator res;

	res.m_ptr		= m_begin;
	res.m_pitch		= m_pitch;
	res.m_is_ptr	= m_is_ptr;

	return res;
}

template <typename T>
inline	typename polymorph_vector_base<T>::iterator		polymorph_vector_base<T>::end	( ) 
{
	typename polymorph_vector_base<T>::iterator res;

	res.m_ptr		= m_end;
	res.m_pitch		= m_pitch;
	res.m_is_ptr	= m_is_ptr;

	return res;
}

template <typename T>
inline	typename polymorph_vector_base<T>::iterator		polymorph_vector_base<T>::begin ( ) const
{
	typename polymorph_vector_base<T>::iterator res;

	res.m_ptr		= m_begin;
	res.m_pitch		= m_pitch;
	res.m_is_ptr	= m_is_ptr;

	return res;
}

template <typename T>
inline	typename polymorph_vector_base<T>::iterator		polymorph_vector_base<T>::end	( ) const
{
	typename polymorph_vector_base<T>::iterator res;

	res.m_ptr		= m_end;
	res.m_pitch		= m_pitch;
	res.m_is_ptr	= m_is_ptr;

	return res;
}



} // namespace render
} // namespace xray


#endif // #ifndef XRAY_RENDER_ENGINE_POLYMORPH_VECTOR_INLINE_H_INCLUDED