////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_ENGINE_H_INCLUDED
#define XRAY_AI_ENGINE_H_INCLUDED

#include <xray/ai/sensed_visual_object.h>

namespace xray {
namespace ai {

struct game_object;
struct sound_player;
struct collision_object;
struct npc;
struct weapon;

typedef vectora< game_object const* >		ai_objects_type;
typedef vectora< weapon* >					weapons_list;

struct XRAY_NOVTABLE engine
{
	virtual	void	get_colliding_objects	( math::aabb const& query_aabb, ai_objects_type& results )	= 0;
	virtual	void	get_visible_objects		( math::cuboid const& cuboid, update_frustum_callback_type const& update_callback ) = 0;
	virtual	bool	ray_query				(
						collision_object const* const object_to_pick,
						collision_object const* const object_to_ignore,
						float3 const& origin,
						float3 const& direction,
						float const max_distance,
						float const transparency_threshold,
						float& visibility_value
					) const = 0;
	virtual	void	draw_frustum			(
							float fov_in_radians,
							float far_plane_distance,
							float aspect_ratio,
							float3 const& position,
							float3 const& direction,
							math::color color
						) const = 0;
	virtual	void	draw_ray				(
						float3 const& start_point,
						float3 const& end_point,
						bool sees_something
					) const = 0;
	virtual	u32		get_node_by_name		( pcstr node_name ) const									= 0;
	virtual	void	get_available_weapons	( npc* owner, weapons_list& list_to_be_filled ) const		= 0;
	
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR	( engine );
}; // struct engine

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_ENGINE_H_INCLUDED
