////////////////////////////////////////////////////////////////////////////
//	Created 	: 15.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOOSE_PTR_INLINE_H_INCLUDED
#define XRAY_LOOSE_PTR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE \
	template < \
		typename object_type, \
		typename base_type, \
		typename threading_policy \
	>

#define LOOSE_PTR \
	xray::loose_ptr< \
		object_type, \
		base_type, \
		threading_policy \
	>

TEMPLATE_SIGNATURE
inline LOOSE_PTR::loose_ptr												( object_type* const object )
{
	m_object		= 0;
	set				( object );
}

TEMPLATE_SIGNATURE
inline LOOSE_PTR::loose_ptr												( self_type const& other )
{
	m_object		= 0;
	set				( other );
}

TEMPLATE_SIGNATURE
inline LOOSE_PTR::~loose_ptr											( )
{
	dec				( );
}

TEMPLATE_SIGNATURE
inline void LOOSE_PTR::dec												( )
{
	if (!m_object)
		return;
	
	if ( !threading_policy::decrement( m_object->base_type::m_reference_count ) )
		m_object->base_type::destroy	( m_object );
}

TEMPLATE_SIGNATURE
inline object_type &LOOSE_PTR::operator*								( ) const
{
	ASSERT			( m_object );
	ASSERT			( m_object->m_pointer, "dangling pointer detected" );
	return			( ( object_type& )*m_object->m_pointer );
}

TEMPLATE_SIGNATURE
inline object_type *LOOSE_PTR::operator->								( ) const
{
	ASSERT			( m_object );
	ASSERT			( m_object->m_pointer, "dangling pointer detected" );
	return			( ( object_type* )m_object->m_pointer );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator!										( ) const
{
	ASSERT			( !m_object || m_object->m_pointer, "dangling pointer detected" );
	return			( !m_object );
}

TEMPLATE_SIGNATURE
inline LOOSE_PTR::operator typename LOOSE_PTR::unspecified_bool_type	( ) const
{
	ASSERT			( !m_object || m_object->m_pointer, "dangling pointer detected" );

	if ( !m_object )
		return		( 0 );

	return			( &loose_ptr::c_ptr );
}

TEMPLATE_SIGNATURE
inline typename LOOSE_PTR::self_type &LOOSE_PTR::operator=				( object_type* const object )
{
	self_type( object ).swap( *this );
	return			( *this );
}

TEMPLATE_SIGNATURE
inline typename LOOSE_PTR::self_type &LOOSE_PTR::operator=				( self_type const& object )
{
	self_type( object ).swap( *this );
	return			( *this );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator==										( LOOSE_PTR const& object ) const
{
	return			( c_ptr( ) == object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator!=										( LOOSE_PTR const& object ) const
{
	return			( c_ptr( ) != object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator<										( LOOSE_PTR const& object ) const
{
	return			( c_ptr( ) < object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator>										( LOOSE_PTR const& object ) const
{
	return			( c_ptr( ) > object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline void LOOSE_PTR::swap												( self_type& object )
{
	std::swap		( m_object, object.m_object );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::equal											( self_type const& object )	const
{
	return			( m_object == object.m_object );
}

TEMPLATE_SIGNATURE
inline void LOOSE_PTR::set												( object_type* const object )
{
	ASSERT			( !m_object || m_object->m_pointer, "dangling pointer detected" );
	if ( m_object && ( ( object_type* )m_object->m_pointer == object ) )
		return;
	
	dec				( );

	m_object		= object ? object->loose_ptr_base::m_pointer : 0;
	if ( !m_object )
		return;

	threading_policy::increment	( m_object->base_type::m_reference_count );
}

TEMPLATE_SIGNATURE
inline void LOOSE_PTR::set												( self_type const& object )
{
	if (m_object == object.m_object)
		return;

	dec				( );
	m_object		= object.m_object;
	
	if (!m_object)
		return;

	threading_policy::increment	( m_object->base_type::m_reference_count );
}

TEMPLATE_SIGNATURE
inline object_type* LOOSE_PTR::c_ptr							( )	const
{
	return			( object_type * )m_object->m_pointer;
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator==								( object_type const* const object ) const
{
	return			( c_ptr() == object );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator!=								( object_type const* const object ) const
{
	return			( c_ptr() != object );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator<								( object_type const* const object ) const
{
	return			( c_ptr() < object );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator<=								( object_type const* const object ) const
{
	return			( c_ptr() <= object );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator>								( object_type const* const object ) const
{
	return			( c_ptr() > object );
}

TEMPLATE_SIGNATURE
inline bool LOOSE_PTR::operator>=								( object_type const* const object ) const
{
	return			( c_ptr() >= object );
}

TEMPLATE_SIGNATURE
inline void swap												( LOOSE_PTR& left, LOOSE_PTR& right )
{
	left.swap		( right );
}

#undef LOOSE_PTR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_LOOSE_PTR_INLINE_H_INCLUDED