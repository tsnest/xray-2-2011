////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BUILDING_OBJECT_H_INCLUDED
#define BUILDING_OBJECT_H_INCLUDED

#include <xray/ai/game_object.h>
#include "ai_collision_object.h"
#include "game_material_visibility_parameters.h"

namespace xray {

namespace ai {
	struct npc;
} // namespace ai

namespace collision {
	class geometry;
	struct space_partitioning_tree;
} // namespace collision

} // namespace xray

namespace stalker2 {

class building_object :
	public ai::game_object,
	private boost::noncopyable
{
public:
			building_object								(
				pcstr name,
				u32 id,
				math::color debug_draw_color,
				collision::space_partitioning_tree& spatial_tree,
				xray::render::scene_ptr const& scene,
				xray::render::debug::renderer& renderer,
				math::float3 const& half_sides,
				math::float3 const& position
			);
			~building_object							( );
	
	virtual	pcstr					get_name			( ) const	{ return m_name.c_str(); } 
	virtual	u32						get_id				( ) const	{ return m_id; } 

	virtual	ai::npc*				cast_npc			( )			{ return 0; }
	virtual	ai::npc const*			cast_npc			( )	const	{ return 0; }
	virtual	ai::weapon*				cast_weapon			( )			{ return 0; }
	virtual	ai::weapon const*		cast_weapon			( )	const	{ return 0; }
	virtual void					draw				( ) const;
	virtual	ai::collision_object*	get_collision_object( ) const;
	virtual float					get_velocity		( ) const	{ return 0.f; }
	virtual float					get_luminosity		( ) const	{ return 0.03f; }
	virtual	float3				get_random_surface_point( u32 const current_time ) const;
	virtual	float4x4				local_to_cell		( float3 const& requester ) const;

			math::aabb				get_aabb			( ) const;

private:
	fixed_string<64>					m_name;
	u32									m_id;
	math::color							m_debug_draw_color;
	float3								m_position;
	game_material_visibility_parameters	m_visibility_parameters;
		
	collision::space_partitioning_tree&	m_spatial_tree;
	collision::geometry_instance*		m_geometry_box;
	ai_collision_object*				m_collision_object;

	xray::render::scene_ptr				m_scene;
	xray::render::debug::renderer&		m_renderer;
}; // class building_object

} // namespace stalker2

#endif // #ifndef BUILDING_OBJECT_H_INCLUDED