////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_BOX_CONTACT_TESTS_H_INCLUDED
#define TRIANGLE_BOX_CONTACT_TESTS_H_INCLUDED

#include <xray/collision/contact_info.h>
#include "triangle_box_res.h"
#include "triangle_mesh_primitive_contact_tests.h"
namespace xray {
namespace collision {
struct on_contact;
class box_geometry_instance;
namespace detail{

class triangle_box_contact_tests:
	public triangle_mesh_primitive_contact_tests<triangle_box_res>,
	private boost::noncopyable
{
private:
	typedef triangle_mesh_primitive_contact_tests<triangle_box_res>		super;

public:
inline			triangle_box_contact_tests	(	const box_geometry_instance &g, const float4x4 &transform,
												buffer_vector<result>& result_buff, on_contact& c );
inline static	math::aabb	get_query_aabb	(  const float4x4 &transform, const box_geometry_instance& og );
public:
	typedef box_geometry_instance geometry_type;
public:
				void	test				( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, float4x4 const& self_transform );
private:
	const box_geometry_instance				&m_geometry;

}; // class triangle_box_contact_tests

}//detail
}//collision
}//xray

#include "triangle_box_contact_tests_inline.h"

#endif // #ifndef TRIANGLE_BOX_CONTACT_TESTS_H_INCLUDED