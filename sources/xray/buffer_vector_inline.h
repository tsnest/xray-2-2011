////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_BUFFER_VECTOR_INLINE_H_INCLUDED
#define XRAY_BUFFER_VECTOR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE	template < typename T >
#define BUFFER_VECTOR		xray::buffer_vector< T >

TEMPLATE_SIGNATURE
inline BUFFER_VECTOR::buffer_vector	( pvoid const buffer, size_type const max_count, size_type const live_count ) :
	m_begin			((T*)buffer),
	m_end			( ((T*)buffer) + live_count )
#ifdef DEBUG
	,m_max_end		((T*)buffer + max_count)
#endif // #ifdef DEBUG
{
	XRAY_UNREFERENCED_PARAMETER	( max_count );
	R_ASSERT		( live_count <= max_count );
}

TEMPLATE_SIGNATURE
inline BUFFER_VECTOR::buffer_vector	( pvoid const buffer, size_type const max_count, size_type const count, value_type const& value ) :
	m_begin			( ( T* )buffer ),
	m_end			( ( T* )buffer )
#ifdef DEBUG
	,m_max_end		( ( T* )buffer + max_count )
#endif // #ifdef DEBUG
{
	XRAY_UNREFERENCED_PARAMETER	( max_count );

	assign			( count, value );
}

TEMPLATE_SIGNATURE
inline BUFFER_VECTOR::buffer_vector	( pvoid const buffer, size_type const max_count, self_type const& other ) :
	m_begin			( ( T* )buffer ),
	m_end			( ( T* )buffer )
#ifdef DEBUG
	,m_max_end		( ( T* )buffer + max_count )
#endif // #ifdef DEBUG
{
#ifndef DEBUG
	XRAY_UNREFERENCED_PARAMETER	( max_count );
#endif // #ifndef DEBUG

	assign			( other.begin( ), other.end( ) );
}

TEMPLATE_SIGNATURE
template < typename input_iterator >
inline BUFFER_VECTOR::buffer_vector	( pvoid const buffer, size_type const max_count, input_iterator const& begin, input_iterator const& end ) :
	m_begin			( ( T* )buffer ),
	m_end			( ( T* )buffer )
#ifdef DEBUG
	,m_max_end		( ( T* )buffer + max_count )
#endif // #ifdef DEBUG
{
#ifndef DEBUG
	XRAY_UNREFERENCED_PARAMETER	( max_count );
#endif // #ifndef DEBUG

	assign			( begin, end );
}

TEMPLATE_SIGNATURE
inline BUFFER_VECTOR::~buffer_vector( )
{
	clear			( );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::self_type &BUFFER_VECTOR::operator=	( self_type const& other )
{
	assign			( other.begin( ), other.end( ) );
	return			( *this );
}

//TEMPLATE_SIGNATURE
//inline bool BUFFER_VECTOR::operator== ( self_type const& other ) const
//{
//	if ( size() != other.size() )
//		return false;
//	
//	for ( size_type i = 0; i < size(); ++i )
//	{
//		if ( operator[]( i ) != other[i] )
//			return false;
//	}
//	return true;
//}

TEMPLATE_SIGNATURE
template < typename input_iterator >
inline void BUFFER_VECTOR::assign	( input_iterator begin, input_iterator const& end )
{
	destroy			( m_begin, m_end );

	m_end			= m_begin + ( end - begin );
	ASSERT			( m_max_end >= m_end );

	for ( iterator I = m_begin; begin != end; ++begin, ++I )
		construct	( I, *begin );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::assign	( size_type const count, non_volatile_const_reference value )
{
	destroy			( m_begin, m_end );

	m_end			= m_begin + count;
	ASSERT			( m_max_end >= m_end );

	for ( iterator I = m_begin; I != m_end; ++I )
		construct	( I, value );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::swap		( self_type& other )
{
//	COMPILE_ASSERT	( false, do_not_use_this_function_for_fixed_and_buffer_vectors );

	std::swap		( m_begin,		other.m_begin );
	std::swap		( m_end,		other.m_end );
#	ifdef DEBUG
		std::swap	( m_max_end,	other.m_max_end );
#	endif // #ifdef DEBUG
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::clear	( )
{
	destroy			( m_begin, m_end );
	m_end			= m_begin;
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::resize	( size_type const size )
{
	size_type		current_size = this->size( );
	if ( size == current_size )
		return;

	if ( size < current_size ) {
		destroy		( m_begin + size, m_begin + current_size );
		m_end		= m_begin + size;
		ASSERT		( m_max_end >= m_end );
		return;
	}

	ASSERT			( size > current_size );
	construct		( m_begin + current_size, m_begin + size );
	m_end			= m_begin + size;
	ASSERT			( m_max_end >= m_end );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::resize	( size_type const size, non_volatile_const_reference value )
{
	size_type		current_size = this->size( );
	if ( size == current_size )
		return;

	if ( size < current_size ) {
		destroy		( m_begin + size, m_begin + current_size );
		m_end		= m_begin + size;
		ASSERT		( m_max_end >= m_end );
		return;
	}

	ASSERT			( size > current_size );
	construct		( m_begin + current_size, m_begin + size, value );
	m_end			= m_begin + size;
	ASSERT			( m_max_end >= m_end );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::reserve	( size_type const size )
{
	XRAY_UNREFERENCED_PARAMETER	(size);
	// nothing to do here
}

TEMPLATE_SIGNATURE
template < typename input_iterator >
inline void BUFFER_VECTOR::insert	( iterator const &where, input_iterator begin, input_iterator const& end )
{
	ASSERT			( where >= m_begin );

	difference_type	count = end - begin;
	ASSERT			( m_end + count <= m_max_end );
	
	{
		iterator		j = m_end + count - 1;		// new end
		iterator		i = m_end - 1;				// old end
		iterator		e = where - 1;
		for ( ; i != e; --i, --j ) {
			construct	( j, *i );
			destroy		( i );
		}
	}

	m_end			+= count;

	for ( iterator i = where, e = i + count; i != e; ++i, ++begin )
		construct	( i, *begin );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::insert	( iterator const& where, size_type const count, non_volatile_const_reference value )
{
	ASSERT			( where >= m_begin );
	ASSERT			( m_end + count <= m_max_end );

	{
		iterator		j = m_end + count - 1;		// new end
		iterator		i = m_end - 1;				// old end
		iterator		e = where - 1;
		for ( ; i != e; --i, --j ) {
			construct	( j, *i );
			destroy		( i );
		}
	}

	m_end			+= count;

	for ( iterator i = where, e = i + count; i != e; ++i )
		construct	( i, value );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::insert	( iterator const &where, non_volatile_const_reference value )
{
	insert			( where, (size_type)1, (non_volatile_const_reference)value );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::erase	( iterator const& begin, iterator const& end )
{
	ASSERT			( m_begin <= begin );
	ASSERT			( m_end >= begin );
	
	ASSERT			( m_begin <= end );
	ASSERT			( m_end >= end );
	
	ASSERT			( begin <= end );
	if ( begin == end )
		return;

	for ( iterator i = begin, j = end; j != m_end; ++i, ++j ) {
		destroy		( i );
		construct	( i, *j );
	}

	size_type		count = size_type(end - begin);
	size_type		size = this->size( );
	ASSERT			( size >= count );
	size_type		new_size = size - count;
	destroy			( m_begin + new_size, m_end );
	m_end			= m_begin + new_size;
	ASSERT			( m_max_end >= m_end );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::erase	( iterator const& where )
{
	erase			( where, where + 1 );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::pop_back	( )
{
	ASSERT			( !empty( ) );
	--m_end;
	destroy			( m_end );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::push_back( non_volatile_const_reference value )
{
	ASSERT			( m_end < m_max_end );
	construct		( m_end, value );
	++m_end;
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::reference BUFFER_VECTOR::at					( size_type const index )
{
	ASSERT			( index < size( ) );
	return			( m_begin[ index ] );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_reference BUFFER_VECTOR::at			( size_type const index ) const
{
	ASSERT			( index < size( ) );
	return			( m_begin[ index ] );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::reference BUFFER_VECTOR::operator [ ]		( size_type const index )
{
	ASSERT			( index < size( ) );
	return			( m_begin[ index ] );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_reference BUFFER_VECTOR::operator[]	( size_type const index ) const
{
	ASSERT			( index < size( ) );
	return			( m_begin[ index ] );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::reference BUFFER_VECTOR::back				( )
{
	ASSERT			( !empty( ) );
	return			( *( m_end - 1 ) );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_reference BUFFER_VECTOR::back			( ) const
{
	ASSERT			( !empty( ) );
	return			( *( m_end - 1 ) );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::reference BUFFER_VECTOR::front				( )
{
	ASSERT			( !empty( ) );
	return			( *m_begin );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_reference BUFFER_VECTOR::front			( ) const
{
	ASSERT			(!empty( ));
	return			( *m_begin );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::iterator BUFFER_VECTOR::begin				( )
{
	return			( m_begin );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_iterator BUFFER_VECTOR::begin			( ) const
{
	return			( m_begin );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::iterator BUFFER_VECTOR::end					( )
{
	return			( m_end );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_iterator BUFFER_VECTOR::end			( ) const
{
	return			( m_end );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::reverse_iterator BUFFER_VECTOR::rbegin		( )
{
	return			( reverse_iterator( end( ) ) );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_reverse_iterator BUFFER_VECTOR::rbegin	( ) const
{
	return			( const_reverse_iterator( end( ) ) );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::reverse_iterator BUFFER_VECTOR::rend			( )
{
	return			( reverse_iterator( begin( ) ) );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::const_reverse_iterator BUFFER_VECTOR::rend	( ) const
{
	return			( const_reverse_iterator( begin( ) ) );
}

TEMPLATE_SIGNATURE
inline bool BUFFER_VECTOR::empty											( ) const
{
	return			( m_begin == m_end );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::size_type BUFFER_VECTOR::size				( ) const
{
	return			( size_type(m_end - m_begin) );
}

#ifdef DEBUG
TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::size_type BUFFER_VECTOR::capacity			( ) const
{
	return			( size_type(m_max_end - m_begin) );
}

TEMPLATE_SIGNATURE
inline typename BUFFER_VECTOR::size_type BUFFER_VECTOR::max_size			( ) const
{
	return			( size_type(m_max_end - m_begin) );
}
#endif // #ifdef DEBUG

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::construct( pointer p)
{
	memory::detail::call_constructor( (non_volatile_pointer)p, (non_volatile_pointer)p+1 );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::construct( pointer p, non_volatile_const_reference value )
{
	new ( (non_volatile_pointer)p ) T		( value );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::construct( iterator begin, iterator const& end )
{
	for ( ; begin != end; ++begin )
		construct	( (non_volatile_pointer)begin );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::construct( iterator begin, iterator const& end, non_volatile_const_reference value )
{
	for ( ; begin != end; ++begin )
		construct	( begin, value );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::destroy	( pointer const p )
{
	XRAY_UNREFERENCED_PARAMETER	( p );
	p->~T			( );
}

TEMPLATE_SIGNATURE
inline void BUFFER_VECTOR::destroy	( iterator begin, iterator const& end )
{
	for ( ; begin != end; ++begin )
		destroy		( (non_volatile_pointer)begin );
}

TEMPLATE_SIGNATURE
inline void swap					( BUFFER_VECTOR& left, BUFFER_VECTOR& right )
{
	left.swap		( right );
}

#undef BUFFER_VECTOR
#undef TEMPLATE_SIGNATURE

#endif // XRAY_CORE_BUFFER_VECTOR_INLINE_H_INCLUDED