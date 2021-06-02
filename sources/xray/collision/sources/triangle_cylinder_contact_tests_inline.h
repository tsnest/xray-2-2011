////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_CYLINDER_CONTACT_TESTS_INLINE_H_INCLUDED
#define TRIANGLE_CYLINDER_CONTACT_TESTS_INLINE_H_INCLUDED

#include "cylinder_geometry_instance.h"

namespace xray {
namespace collision {

struct on_contact;

namespace detail{

inline	triangle_cylinder_contact_tests::triangle_cylinder_contact_tests	(	const cylinder_geometry_instance &g, const float4x4 &transform,
																			 buffer_vector<result>& result_buff, on_contact& c ):
	super( transform,  result_buff, c ),
	m_geometry(g)
{

}
inline math::aabb triangle_cylinder_contact_tests::get_query_aabb			(  float4x4 const& transform, cylinder_geometry_instance const& og )
{
	XRAY_UNREFERENCED_PARAMETER( transform );
	return	og.get_aabb( );
}





}//detail {
}//collision{
}//xray{

#endif // #ifndef TRIANGLE_CYLINDER_CONTACT_TESTS_INLINE_H_INCLUDED