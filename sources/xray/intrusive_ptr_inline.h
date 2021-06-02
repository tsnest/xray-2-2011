////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INTRUSIVE_PTR_INLINE_H_INCLUDED
#define XRAY_INTRUSIVE_PTR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE \
	template <\
		typename ObjectType,\
		typename BaseType,\
		typename policy_type\
	>

#define INTRUSIVE_PTR \
	xray::intrusive_ptr<\
		ObjectType,\
		BaseType,\
		policy_type\
	>

TEMPLATE_SIGNATURE
inline INTRUSIVE_PTR::intrusive_ptr												( )
{
	m_object		= 0;
}

TEMPLATE_SIGNATURE
inline INTRUSIVE_PTR::intrusive_ptr												( object_type* const object )
{
	m_object		= 0;
	set				( object );
}

TEMPLATE_SIGNATURE
inline INTRUSIVE_PTR::intrusive_ptr												( self_type const& other )
{
	m_object		= 0;
	set				( other );
}

TEMPLATE_SIGNATURE
inline INTRUSIVE_PTR::~intrusive_ptr											( )
{
	dec				( );
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_PTR::dec													( )
{
	if (!m_object)
		return;

	if ( !policy_type::intrusive_ptr_decrement( static_cast<BaseType&>(*m_object) ) )
		m_object->BaseType::destroy	( m_object );
}

TEMPLATE_SIGNATURE
inline ObjectType &INTRUSIVE_PTR::operator*									( ) const
{
	ASSERT			( m_object );
	return			( *m_object );
}

TEMPLATE_SIGNATURE
inline ObjectType *INTRUSIVE_PTR::operator->									( ) const
{
	ASSERT			( m_object );
	return			( m_object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator!	( ) const
{
	return			( !m_object );
}

TEMPLATE_SIGNATURE
inline INTRUSIVE_PTR::operator typename INTRUSIVE_PTR::unspecified_bool_type	( ) const
{
	if ( !m_object )
		return		( 0 );

	return			( &intrusive_ptr::c_ptr );
}

TEMPLATE_SIGNATURE
inline typename INTRUSIVE_PTR::self_type &INTRUSIVE_PTR::operator=				( object_type* const object )
{
	self_type( object ).swap( *this );
	return			( *this );
}

TEMPLATE_SIGNATURE
inline typename INTRUSIVE_PTR::self_type &INTRUSIVE_PTR::operator=				( self_type const& object )
{
	self_type( object ).swap( *this );
	return			( *this );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator==											( self_type const& object ) const
{
	return			( c_ptr( ) == object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator!=											( self_type const& object ) const
{
	return			( c_ptr( ) != object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator<											( self_type const& object ) const
{
	return			( c_ptr( ) < object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator<=											( self_type const& object ) const
{
	return			( c_ptr( ) <= object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator>											( self_type const& object ) const
{
	return			( c_ptr( ) > object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator>=											( self_type const& object ) const
{
	return			( c_ptr( ) >= object.c_ptr( ) );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator==											( object_type const* const object ) const
{
	return			( c_ptr() == object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator!=											( object_type const* const object ) const
{
	return			( c_ptr() != object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator<											( object_type const* const object ) const
{
	return			( c_ptr() < object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator<=											( object_type const* const object ) const
{
	return			( c_ptr() <= object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator>											( object_type const* const object ) const
{
	return			( c_ptr() > object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::operator>=											( object_type const* const object ) const
{
	return			( c_ptr() >= object );
}


TEMPLATE_SIGNATURE
inline void INTRUSIVE_PTR::swap													( self_type& object )
{
	std::swap		( m_object, object.m_object );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::equal												( self_type const& object )	const
{
	return			( c_ptr() == object.c_ptr() );
}

TEMPLATE_SIGNATURE
inline bool INTRUSIVE_PTR::equal												( object_type* const object )	const
{
	return			( c_ptr() == object );
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_PTR::set													( object_type* const object )
{
	if ( equal(object) )
		return;
	
	dec				( );

	m_object		= object;
	if (!m_object)
		return;

	policy_type::intrusive_ptr_increment( static_cast<BaseType&>(*m_object) );
}

TEMPLATE_SIGNATURE
inline void INTRUSIVE_PTR::set													( self_type const& object )
{
	if (m_object == object.m_object)
		return;

	dec				( );
	m_object		= object.m_object;
	
	if (!m_object)
		return;

	policy_type::intrusive_ptr_increment	( static_cast<BaseType&>(*m_object) );
}

TEMPLATE_SIGNATURE
inline ObjectType* INTRUSIVE_PTR::c_ptr										( )	const
{
	return			( m_object );
}

TEMPLATE_SIGNATURE
inline void swap																( INTRUSIVE_PTR& left, INTRUSIVE_PTR& right )
{
	left.swap		( right );
}


template <
	typename DestinationType,
	typename ObjectType,
	typename BaseType,
	typename policy_type
>
DestinationType xray::static_cast_resource_ptr				(
		xray::intrusive_ptr<
			ObjectType,
			BaseType,
			policy_type
		> const& source
	)
{
	return			static_cast_checked< typename DestinationType::object_type* >( source.c_ptr() );
}

#undef INTRUSIVE_PTR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_INTRUSIVE_PTR_INLINE_H_INCLUDED