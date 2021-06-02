////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_BOX_CONTACT_TESTS_INLINE_H_INCLUDED
#define TRIANGLE_BOX_CONTACT_TESTS_INLINE_H_INCLUDED
#include "box_geometry_instance.h"
namespace xray {
namespace collision {
namespace detail{
inline	triangle_box_contact_tests::triangle_box_contact_tests(	const box_geometry_instance &g, const float4x4 &transform,
																 buffer_vector<result>& result_buff, on_contact& c ):
	super( transform,  result_buff, c ),
	m_geometry(g)
{
	m_radius = g.get_matrix().get_scale().length();
}
	

inline math::aabb triangle_box_contact_tests::get_query_aabb(  const float4x4 &transform, const box_geometry_instance& og )
{
	float3 const& size =
		abs( transform.i.xyz() * og.get_matrix().get_scale().x ) +  
		abs( transform.j.xyz() * og.get_matrix().get_scale().y ) + 
		abs( transform.k.xyz() * og.get_matrix().get_scale().z );

	return	math::create_aabb_center_radius( transform.c.xyz(), size );
}


}//xray{
}//collision{
}//detail{
#endif // #ifndef TRIANGLE_BOC_CONTACT_TESTS_INLINE_H_INCLUDED