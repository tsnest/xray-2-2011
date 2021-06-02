////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_CYLINDER_CONTACT_TESTS_H_INCLUDED
#define TRIANGLE_CYLINDER_CONTACT_TESTS_H_INCLUDED
#include <xray/collision/contact_info.h>
#include "triangle_cylinder_res.h"
#include "triangle_mesh_primitive_contact_tests.h"
namespace xray {
namespace collision {
struct on_contact;
class cylinder_geometry_instance;
namespace detail{

class triangle_cylinder_contact_tests:
	public triangle_mesh_primitive_contact_tests<triangle_cylinder_res>,
	private boost::noncopyable
{
private:
	typedef triangle_mesh_primitive_contact_tests<triangle_cylinder_res>	super;

public:
inline			triangle_cylinder_contact_tests	(	const cylinder_geometry_instance &g, const float4x4 &transform,
												buffer_vector<result>& result_buff, on_contact& c );
inline static	math::aabb	get_query_aabb		(  const float4x4 &transform, const cylinder_geometry_instance& og );
public:
	typedef cylinder_geometry_instance geometry_type;
public:
			void	test				( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, float4x4 const& self_transform );
private:
	const geometry_type					&m_geometry;

}; // class triangle_cylinder_contact_tests
}//detail {
}//collision{
}//xray{
#include "triangle_cylinder_contact_tests_inline.h"
#endif // #ifndef TRIANGLE_CYLINDER_CONTACT_TESTS_H_INCLUDED