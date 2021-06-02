////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RAY_RESULT_H_INCLUDED
#define RAY_RESULT_H_INCLUDED

namespace xray {
namespace physics {

class bt_rigid_body;

struct closest_ray_result
{
	closest_ray_result():m_object(NULL){};

	bt_rigid_body*	m_object;
	float3			m_hit_point_world;
};


} // namespace physics
} // namespace xray

#endif // #ifndef RAY_RESULT_H_INCLUDED