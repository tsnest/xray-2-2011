////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_NON_NULL_PTR_INLINE_H_INCLUDED
#define XRAY_NON_NULL_PTR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE			template < typename type, typename pointer_type >
#define NON_NULL_PTR				xray::non_null_ptr< type, pointer_type >

TEMPLATE_SIGNATURE
inline NON_NULL_PTR::non_null_ptr	( pointer_type const value ) :
	super		( value )
{\
	ASSERT		( super::m_value );\
}

TEMPLATE_SIGNATURE
template < typename type2, typename pointer_type2 >
inline NON_NULL_PTR::non_null_ptr	( raw_ptr< type2, pointer_type2 > const& value ) :
	super		( value )
{\
	ASSERT		( super::m_value );\
}

TEMPLATE_SIGNATURE
template < typename type2, typename pointer_type2 >
inline NON_NULL_PTR::non_null_ptr	( non_null_ptr< type2, pointer_type2 > const& value ) :
	super	( value )
{\
	ASSERT		( super::m_value );\
}

TEMPLATE_SIGNATURE
template < typename type2, typename base_type, typename threading_policy >
inline NON_NULL_PTR::non_null_ptr	( loose_ptr< type2, base_type, threading_policy > const& value ) :
	super		( value )
{\
	ASSERT		( super::m_value );\
}

#undef NON_NULL_PTR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_NON_NULL_PTR_INLINE_H_INCLUDED