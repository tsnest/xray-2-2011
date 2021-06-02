////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_MESH_PRIMITIVE_CONTACT_TESTS_H_INCLUDED
#define TRIANGLE_MESH_PRIMITIVE_CONTACT_TESTS_H_INCLUDED
#include "triangle_mesh_primitiv_res.h"
namespace xray {
namespace collision {
struct on_contact;
namespace detail{
template<class T>
class triangle_mesh_primitive_contact_tests {
public:
	typedef	T	result;

	inline triangle_mesh_primitive_contact_tests(	const float4x4 &transform,
													buffer_vector<result>& result_buff, on_contact& c );
protected:
	inline ~triangle_mesh_primitive_contact_tests( );

public:
	inline	bool	trash_back			( const result &back_tri, float3 const* verts, const float4x4 &seff_transform );
	inline	void	back_test			( result &back_tri, float3 const* verts, const float4x4 &seff_transform );
protected:
			void	test				( buffer_vector<u32> &backs, const u32 * ind, float3 const* verts, const float4x4 &seff_transform );
protected:
	float4x4							m_transform;
	buffer_vector<result>				&res_vec;
	on_contact							&cb;
	back_res							back_res;
	float								m_radius;
}; // class triangle_mesh_primitive_contact_tests

}//detail
}//collision
}//xray
#include "triangle_mesh_primitive_contact_tests_inline.h"
#endif // #ifndef TRIANGLE_PRIMITIVE_CONTACT_TESTS_H_INCLUDED