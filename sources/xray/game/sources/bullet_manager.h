////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BULLET_MANAGER_H_INCLUDED
#define BULLET_MANAGER_H_INCLUDED

#include <xray/memory_single_size_buffer_allocator.h>
#include <xray/unmanaged_allocation_resource.h>
#include "bullet.h"

#ifndef MASTER_GOLD

#include <xray/render/facade/debug_renderer.h>

#endif // #ifndef MASTER_GOLD

namespace xray{
	namespace render{
		struct material_effects_instance_cook_data;
		struct vertex_colored;
	}
	namespace tasks {
		class task_type;
	}
}

namespace stalker2
{

class	game;
struct	bullet_ticker_predicate;

class bullet_manager 
{
public:
					bullet_manager					( game& game );
					~bullet_manager					( );
	void			initialize						( );
	void			register_console_commands		( );

public:
	void			tick							( float elapsed_game_seconds );
	game&			get_game						( ) const;
	float3 const&	get_gravity						( ) const;
	void			fire							( float3 position, float3 direction );
	float			get_bullet_time_factor			( );
	void			add_decal						( float3 const& position, float3 const& direction, float3 const& normal, bool is_front_face );

#ifndef MASTER_GOLD
	void			render_debug					( );
	void			store_bullet_trajectory			( bullet* bullet );
	void			toggle_is_fixed					( );
	void			add_collision_point				( float3 const& point, math::color const& color );
#endif // #ifndef MASTER_GOLD		

private:
	void			tick_bullets					( u32 start_index, u32 end_index, float elapsed_game_seconds );
	void			displace_all_bullets			( pcstr args );
	void			set_max_bullets					( pcstr args );

	void			allocate_bullets_memory			( u32 new_max_bullets_count );
	void			bullets_memory_allocated		( resources::queries_result& queries );
	void			request_bullet_decal_material	( );
	void			material_ready					( resources::queries_result& data, xray::render::material_effects_instance_cook_data* cook_data );
	void			material_ready_out				( resources::queries_result& data, xray::render::material_effects_instance_cook_data* cook_data );

	void			emit_bullet						( float3 position, float3 direction, float air_resistance );
	void			destroy_bullet					( buffer_vector<bullet*>::iterator& bullet );
	void			displace_one_bullet				( );

private:
	typedef memory::single_size_buffer_allocator< sizeof( bullet ), threading::simple_lock > bullets_allocator;
	typedef xray::resources::unmanaged_allocation_resource_ptr		unmanaged_allocation_resource_ptr;
	typedef xray::resources::unmanaged_resource_ptr					unmanaged_resource_ptr;
	typedef buffer_vector<bullet*>									bullets_type;

private:
	bullets_type									m_bullets;
	float3											m_gravity;

	unmanaged_resource_ptr							m_material_ptr;
	unmanaged_resource_ptr							m_material_ptr_out;
	unmanaged_allocation_resource_ptr				m_bullets_memory_ptr;
	uninitialized_reference< bullets_allocator >	m_bullets_allocator_ref;

	game&											m_game;
	tasks::task_type*								m_task_type;

	u32												m_max_bullets_count;
	u32												m_max_bullets_decals_count;
	u32												m_current_decal_id;

	float											m_bullet_time_factor;
	float											m_air_resistance_epsilon;
	float											m_current_air_resistance;

#ifndef MASTER_GOLD

public:
	struct decal_data
	{
		decal_data( float3 pos, float3 dir, float3 n, float d ):
			position	( pos ),
			direction	( dir ),
			normal		( n ),
			depth		( d ){}

		float3	position;
		float3	direction;
		float3	normal;
		float	depth;
	}; // struct decal_data

private:
	debug::vector< float3 >							m_collision_points;
	debug::vector< math::color >					m_collision_point_colors;
	debug::vector< decal_data >						m_decals;
	vectora< render::vertex_colored >				m_bullet_trajectories_points;
	debug::vector< u32 >							m_bullet_sequences_sizes;

	float3											m_fixed_position;
	float3											m_fixed_direction;
	bool											m_is_fixed;

	bool											m_is_draw_debug;
	bool											m_is_draw_trajectories;
	bool											m_is_draw_collision_trajectories;
	bool											m_is_draw_decals_data;
	bool											m_is_draw_collision_points;
#endif // #ifndef MASTER_GOLD
}; // class bullet_manager

} // namespace stalker2

#endif // #ifndef BULLET_MANAGER_H_INCLUDED