////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.03.2008
//	Author		: Konstantin Slipchenko
//	Description : this needed to generalize triangle_mesh - prmitive contact generation procedure
////////////////////////////////////////////////////////////////////////////
#ifndef TRIANGLE_SPHERE_CONTACT_TESTS_H_INCLUDED
#define TRIANGLE_SPHERE_CONTACT_TESTS_H_INCLUDED
#include "triangle_sphere.h"
#include "sphere_geometry_instance.h"

namespace xray {
namespace collision {
struct on_contact;
namespace detail{

class triangle_sphere_contact_tests
{
public:
typedef	triangle_sphere_res result;
inline			triangle_sphere_contact_tests	( const sphere_geometry_instance	 &g, const float4x4 &transform,
												 buffer_vector<result>& result_buff, on_contact& c );

inline static	math::aabb get_query_aabb		(  const float4x4 &transform, const xray::collision::sphere_geometry_instance& og );
public:
	
	typedef sphere_geometry_instance geometry_type;
public:
	inline	void	test				( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, const float4x4 &mesh_transform );
	inline	bool	trash_back			( const result &back_tri, float3 const* verts, const float4x4 &mesh_transform );
	inline	void	back_test			( result &back_tri, float3 const* verts, const float4x4 &mesh_transform  );
private:
	const sphere_geometry_instance		&m_geometry;
	const float4x4						&m_transform;

	triangle_sphere_test_callback		cbr;


};

}//detail
}//collision
}//xray

#include "triangle_sphere_contact_tests_inline.h"

#endif