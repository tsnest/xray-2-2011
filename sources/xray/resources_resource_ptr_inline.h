////////////////////////////////////////////////////////////////////////////
//	Created		: 03.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_RESOURCE_PTR_INLINE_H_INCLUDED
#define XRAY_RESOURCES_RESOURCE_PTR_INLINE_H_INCLUDED

#define TEMPLATE_SIGNATURE template < typename object_type, typename base_type >
#define RESOURCE_PTR resource_ptr< object_type, base_type >

namespace xray {
namespace resources {

TEMPLATE_SIGNATURE
inline RESOURCE_PTR::resource_ptr (object_type * const object) : super(object)		
{
}

TEMPLATE_SIGNATURE
inline RESOURCE_PTR::resource_ptr (self_type const & other) : super(other)
{
}

TEMPLATE_SIGNATURE
inline RESOURCE_PTR &   RESOURCE_PTR::operator = (object_type * const object)
{
	super::operator =	(object);
	return				* this;
}

TEMPLATE_SIGNATURE
inline RESOURCE_PTR &   RESOURCE_PTR::operator = (self_type const & object)
{
	super::operator =	(object);
	return				* this;
}

// TEMPLATE_SIGNATURE
// inline void RESOURCE_PTR::swap													( self_type& object )
// {
// 	std::swap		((super &) * this, (super &)object);
// }
// 
// TEMPLATE_SIGNATURE
// void   swap (RESOURCE_PTR & left, RESOURCE_PTR & right)
// {
// 	left.swap		( right );
// }

} // namespace resources

template < typename dest_type, typename object_type, typename base_type >
inline dest_type static_cast_resource_ptr	( resources::resource_ptr< object_type, base_type > const src_ptr)
{
	return			static_cast_checked<typename dest_type::object_type*>(src_ptr.c_ptr());
}

} // namespace xray

#undef RESOURCE_PTR
#undef TEMPLATE_SIGNATURE

#endif // #ifndef XRAY_RESOURCES_RESOURCE_PTR_INLINE_H_INCLUDED