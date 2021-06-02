////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RAW_PTR_INLINE_H_INCLUDED
#define XRAY_RAW_PTR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE						template < typename type, typename pointer_type >
#define RAW_PTR									xray::raw_ptr< type, pointer_type >

TEMPLATE_SIGNATURE
inline RAW_PTR::raw_ptr							( pointer_type const value ) :
	m_value		( value )
{\
}

TEMPLATE_SIGNATURE
template < typename type2, typename pointer_type2 >
inline RAW_PTR::raw_ptr							( raw_ptr< type2, pointer_type2 > const& value ) :
	m_value		( value ? &*value : 0 )
{\
}

TEMPLATE_SIGNATURE
template < typename type2, typename pointer_type2 >
inline RAW_PTR::raw_ptr							( non_null_ptr< type2, pointer_type2 > const& value ) :
	m_value		( value ? &*value : 0 )
{\
}

TEMPLATE_SIGNATURE
template < typename type2, typename base_type, typename threading_policy >
inline RAW_PTR::raw_ptr							( loose_ptr< type2, base_type, threading_policy > const& value ) :
	m_value		( value ? &*value : 0 )
{\
}

TEMPLATE_SIGNATURE
inline type* RAW_PTR::operator ->				( ) const
{\
	ASSERT		( m_value );\
	return		( &*m_value );\
}

TEMPLATE_SIGNATURE
inline type& RAW_PTR::operator *				( ) const
{\
	ASSERT		( m_value );\
	return		( (type&)*m_value );\
}

TEMPLATE_SIGNATURE
inline bool	RAW_PTR::operator !					( ) const
{\
	return		( !m_value );\
}

TEMPLATE_SIGNATURE
inline RAW_PTR::operator unspecified_bool_type	( ) const
{\
	if ( !m_value )\
		return	( 0 );\
\
	return		( &RAW_PTR::operator-> );\
}

TEMPLATE_SIGNATURE
inline RAW_PTR& RAW_PTR::operator =				( type* const object )
{
	self_type( object ).swap( *this );
	return		( *this );
}

TEMPLATE_SIGNATURE
inline RAW_PTR& RAW_PTR::operator =				( self_type const& object )
{\
	self_type( object ).swap( *this );\
	return		( *this );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::operator ==				( self_type const& object ) const
{\
	return		( m_value == object.m_value );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::operator !=				( self_type const& object ) const
{\
	return		( m_value != object.m_value );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::operator <					( self_type const& object ) const
{\
	return		( m_value < object.m_value );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::operator <=				( self_type const& object ) const
{\
	return		( m_value <= object.m_value );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::operator >					( self_type const& object ) const
{\
	return		( m_value > object.m_value );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::operator >=				( self_type const& object ) const
{\
	return		( m_value > object.m_value );\
}

TEMPLATE_SIGNATURE
inline void RAW_PTR::swap						( self_type& object )
{\
	std::swap	( m_value, object.m_value );\
}

TEMPLATE_SIGNATURE
inline bool RAW_PTR::equal						( self_type const& object )	const
{\
	return		( m_value == object.m_value );\
}

namespace xray {

TEMPLATE_SIGNATURE
inline void swap								( RAW_PTR& left, RAW_PTR& right )
{
	left.swap	( right );
}

template < typename inherited_type, typename type, typename pointer_type >
inline bool operator ==							( inherited_type* const left, RAW_PTR const& right )
{\
	return		( RAW_PTR(left) == right );\
}

template < typename inherited_type, typename type, typename pointer_type >
inline bool operator !=							( inherited_type* const left, RAW_PTR const& right )
{\
	return		( RAW_PTR(left) != right );\
}

template < typename inherited_type, typename type, typename pointer_type >
inline bool operator <							( inherited_type* const left, RAW_PTR const& right )
{\
	return		( RAW_PTR(left) < right );\
}

template < typename inherited_type, typename type, typename pointer_type >
inline bool operator <=							( inherited_type* const left, RAW_PTR const& right )
{\
	return		( RAW_PTR(left) <= right );\
}

template < typename inherited_type, typename type, typename pointer_type >
inline bool operator >							( inherited_type* const left, RAW_PTR const& right )
{\
	return		( RAW_PTR(left) > right );\
}

template < typename inherited_type, typename type, typename pointer_type >
inline bool operator >=							( inherited_type* const left, RAW_PTR const& right )
{\
	return		( RAW_PTR(left) >= right );\
}

} // namespace xray

#undef RAW_PTR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_RAW_PTR_INLINE_H_INCLUDED