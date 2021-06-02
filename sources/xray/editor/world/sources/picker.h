////////////////////////////////////////////////////////////////////////////
//	Created		: 13.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_HELPER_H_INCLUDED
#define COLLISION_HELPER_H_INCLUDED

#pragma managed(push,off)
#	include <xray/collision/common_types.h>
#pragma managed(pop)

namespace xray {

namespace collision { struct space_partitioning_tree; }

namespace resources { class queries_result; }

namespace editor {

class editor_world;
class collision_object;

class picker {
public:
	picker( editor_world* world );

	bool ray_query( xray::collision::object_type const mask, collision_object const** object, bool check_priority, float3* position = NULL );
	bool ray_query( xray::collision::object_type const mask, xray::math::int2 screen_xy, collision_object const** object, bool check_priority, float3* position = NULL );
	bool ray_query( xray::collision::object_type const mask, xray::math::float3 origin, xray::math::float3 direction, collision_object const** object, bool check_priority, float3* position = NULL );
	bool ray_query( xray::collision::object_type const mask, xray::math::float3 origin, xray::math::float3 direction, collision::ray_objects_type& collision_results );

	bool frustum_query( xray::collision::object_type const mask, xray::math::int2 min_xy, xray::math::int2 max_xy, vector< collision_object const*> &objects );

	void clear_cache();

private:
	editor_world*			m_world;
	xray::math::int2		m_chached_pick_xy;
	float3					m_cached_pos;
	bool					m_cached;
	bool					m_cached_result;
	bool					m_cached_priority_flag;
	xray::collision::object_type m_cached_mask;
	collision_object const* m_cached_obj;
}; // class picker

} // namespace editor
} // namespace xray

#endif // #ifndef COLLISION_HELPER_H_INCLUDED