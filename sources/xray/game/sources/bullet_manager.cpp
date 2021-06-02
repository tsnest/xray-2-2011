////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bullet_manager.h"
#include "bullet.h"
#include "game.h"
#include "game_world.h"
#include "game_camera.h"
#include "bullet_manager_input_handler.h"

#include <xray/tasks_system.h>
#include <xray/buffer_vector.h>
#include <xray/console_command.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/render/facade/decal_properties.h>
#include <xray/render/facade/material_effects_instance_cook_data.h>
#include <xray/render/facade/vertex_input_type.h>

#ifndef MASTER_GOLD

#include <xray/collision/common_types.h>
#include <xray/collision/space_partitioning_tree.h>
#include "collision_object_types.h"

#endif // #ifndef MASTER_GOLD

namespace stalker2 {

/////////////////////////////			   I N I T I A L I Z E				///////////////////////////////////////
bullet_manager::bullet_manager ( game& game ) :
	m_bullets				( NULL, 0 ),
	m_gravity					( float3( 0, -9.8f, 0 ) ),
	m_game						( game ),
	m_task_type					( tasks::create_new_task_type( "bullet", tasks::task_type_flags_no_self_parallelization_hint ) ),
	m_max_bullets_count			( 0 ),
	m_max_bullets_decals_count	( 64 ),
	m_current_decal_id			( 0 ),
	m_bullet_time_factor		( 1 ),
	m_air_resistance_epsilon	( .1f ),
	m_current_air_resistance	( 1 )

#ifndef MASTER_GOLD	
	,
	m_is_draw_debug				( true ),
	m_is_draw_trajectories		( true ),
	m_is_draw_collision_trajectories( false ),
	m_is_draw_decals_data		( true ),
	m_is_draw_collision_points	( true ),
	m_is_fixed					( false ),
	m_bullet_trajectories_points( vectora< render::vertex_colored >( g_allocator ) )

#endif // #ifndef MASTER_GOLD
{
	initialize					( );
	register_console_commands	( );

	static bullet_manager_input_handler	input_handler( *this );
}

bullet_manager::~bullet_manager ( )
{
	if( m_bullets_allocator_ref.initialized( ) )
		XRAY_DESTROY_REFERENCE		( m_bullets_allocator_ref );
}

void bullet_manager::initialize ( )
{
	allocate_bullets_memory			( 32 );

	request_bullet_decal_material	( );
}

void bullet_manager::register_console_commands ( )
{
	static xray::console_commands::cc_delegate	displace_all_bullets_cc				( "bullets_manager_displace_all_bullets",		boost::bind( &bullet_manager::displace_all_bullets, this, _1 ), true );
	static xray::console_commands::cc_delegate	set_max_bullets_cc					( "bullets_manager_set_max_bullets",			boost::bind( &bullet_manager::set_max_bullets, this, _1 ), true );
	static xray::console_commands::cc_float		set_time_factor_cc					( "bullets_manager_set_time_factor",			m_bullet_time_factor, 0.0f, 10.0f, true, xray::console_commands::command_type_user_specific );

#ifndef MASTER_GOLD

	static xray::console_commands::cc_bool		is_draw_debug_cc					( "bullets_manager_draw_debug",					m_is_draw_debug, true, xray::console_commands::command_type_user_specific );
	static xray::console_commands::cc_bool		is_draw_trajectories_cc				( "bullets_manager_draw_trajectories",			m_is_draw_trajectories, true, xray::console_commands::command_type_user_specific );
	static xray::console_commands::cc_bool		is_draw_collision_trajectories_cc	( "bullets_manager_draw_collision_trajectories",m_is_draw_collision_trajectories, true, xray::console_commands::command_type_user_specific );
	static xray::console_commands::cc_bool		is_draw_decal_data_cc				( "bullets_manager_draw_decals_data",			m_is_draw_decals_data, true, xray::console_commands::command_type_user_specific );
	static xray::console_commands::cc_bool		is_draw_collision_points_cc			( "bullets_manager_draw_collision_points",		m_is_draw_collision_points, true, xray::console_commands::command_type_user_specific );
	static xray::console_commands::cc_float		set_air_ersistance_cc				( "bullets_manager_air_resistance",				m_current_air_resistance, 0.01f, 20.0f, true, xray::console_commands::command_type_user_specific );

#endif // #ifndef MASTER_GOLD

}


/////////////////////////////		   P U B L I C   M E T H O D S			///////////////////////////////////////

struct redundant_bullet_predicate
{
public:
	bool operator ()( bullet* bullet )
	{
		return bullet->is_finish_flying( );
	}
};

void bullet_manager::tick ( float elapsed_game_seconds )
{
	u32 const bullets_count		= m_bullets.size();
	if ( bullets_count ) {
		u32 const granularity	= 16;
		u32 const n				= bullets_count/granularity;
		for ( u32 i = 0; i < n; ++i )
			tasks::spawn_task	( boost::bind( &bullet_manager::tick_bullets, this, i*16, (i + 1)*16 , elapsed_game_seconds ), m_task_type );

		u32 const start_index	= n * granularity;
		tick_bullets			( start_index, start_index + bullets_count % granularity, elapsed_game_seconds );

		tasks::wait_for_all_children	( );

		m_bullets.erase			(
			std::remove_if(
				m_bullets.begin( ),
				m_bullets.end( ),
				redundant_bullet_predicate( )
			),
			m_bullets.end( )
		);
	}

#ifndef MASTER_GOLD
	if( m_is_draw_debug )
		render_debug			( );
#endif // #ifndef MASTER_GOLD
}

game& bullet_manager::get_game ( ) const
{
	return m_game;
}

float3 const& bullet_manager::get_gravity						( ) const
{
	return m_gravity;
}

void bullet_manager::fire ( float3 position, float3 direction )
{
#ifndef MASTER_GOLD

	if( m_is_fixed )
		emit_bullet( m_fixed_position, m_fixed_direction * 900, m_current_air_resistance );
	else

#endif // #ifndef MASTER_GOLD
		emit_bullet( position, direction, m_current_air_resistance );
}

float bullet_manager::get_bullet_time_factor ( )
{
	return m_bullet_time_factor;
}
void bullet_manager::add_decal ( float3 const& position, float3 const& direction, float3 const& normal, bool is_front_face )
{
	xray::render::decal_properties				properties;
	float4x4	transform						= xray::math::create_rotation( -normal, ( direction ^ normal ).normalize( ) );

	float decal_depth							= .1f;
	transform.c.xyz( )							= position + normal * decal_depth / 2;

#ifndef MASTER_GOLD

	m_decals.push_back( decal_data( position, direction, normal, decal_depth ) );
	if( m_decals.size( ) > 20 )
		m_decals.clear( );

#endif // #ifndef MASTER_GOLD

	properties.material							= ( is_front_face ) ? m_material_ptr : m_material_ptr_out;
	properties.transform						= transform;
	properties.transform.set_scale				( float3( 1.0f, 1.0f, 1.0f ) );
	properties.projection_on_static_geometry	= true;
	properties.projection_on_skeleton_geometry	= true;
	properties.projection_on_terrain_geometry	= true;
	properties.projection_on_speedtree_geometry	= true;
	properties.projection_on_particle_geometry	= true;
	properties.width_height_far_distance		= 0.5f * float3( .2f, .2f, decal_depth );
	properties.alpha_angle						= 0.0f;
	properties.clip_angle						= -90.0f;
	
	m_game.renderer( ).scene( ).update_decal( m_game.get_render_scene( ), m_current_decal_id++, properties );

	if( m_current_decal_id == m_max_bullets_decals_count )
		m_current_decal_id = 0;
}

/////////////////////////////		   D E B U G   M E T H O D S			///////////////////////////////////////

#ifndef MASTER_GOLD

void bullet_manager::store_bullet_trajectory ( bullet* bullet )
{
	if( m_bullet_sequences_sizes.size( ) > 10 )
	{
		m_bullet_sequences_sizes.clear		( );
		m_bullet_trajectories_points.clear	( );
	}

	math::color	path_color		= math::color( 255, 0, 255 );
	math::color	fall_down_color	= math::color( 0, 0, 255 );
	m_bullet_trajectories_points.push_back		( render::vertex_colored( bullet->get_start_position( ), path_color ) );

	bool	just_fall_down = true;
	u32		senquence_size = 1;

	for( u16 i = 1; i <= 400; ++i ) 
	{
		float fall_down			= 0;
		float3 position			= bullet->evaluate_position( i / 100.0f, fall_down, m_gravity );
		if( fall_down < 0 )
		{
			m_bullet_trajectories_points.push_back		( render::vertex_colored( position, path_color ) );
			++senquence_size;
		}
		else
		{
			if( just_fall_down )
			{
				m_bullet_trajectories_points.push_back		( render::vertex_colored( position, path_color ) );
				++senquence_size;
				just_fall_down			= false;
			}

			m_bullet_trajectories_points.push_back		( render::vertex_colored( position, fall_down_color ) );
			++senquence_size;
		}
	}
	m_bullet_sequences_sizes.push_back( senquence_size );
}

void bullet_manager::toggle_is_fixed ( )
{
	m_is_fixed = !m_is_fixed;
	if( m_is_fixed )
	{
		float4x4 view_transform		= m_game.get_game_world( ).get_camera_director( )->get_active_camera( )->get_inverted_view_matrix( );
		m_fixed_position			= view_transform.c.xyz( );
		m_fixed_direction			= view_transform.k.xyz( );
	}
}

void bullet_manager::add_collision_point ( float3 const& point, math::color const& color )
{
	if( m_collision_points.size( ) > 20 )
	{
		m_collision_points.clear( );
		m_collision_point_colors.clear( );
	}

	m_collision_points.push_back( point );
	m_collision_point_colors.push_back( color );
}


#endif // #ifndef MASTER_GOLD

/////////////////////////////		  P R I V A T E   M E T H O D S			///////////////////////////////////////

void bullet_manager::tick_bullets ( u32 start_index, u32 end_index, float elapsed_game_seconds )
{
	bullets_type::iterator			current = m_bullets.begin( ) + start_index;
	bullets_type::iterator const	end		= m_bullets.begin( ) + end_index;

	for ( ; current != end; ++current )
		(*current)->tick( *this, elapsed_game_seconds );
}
void bullet_manager::displace_all_bullets ( pcstr args )
{
	XRAY_UNREFERENCED_PARAMETER( args );
	for( ; !m_bullets.empty( ); )
	{
		bullets_type::iterator iter = m_bullets.end( );
		destroy_bullet( iter );
	}
}
void bullet_manager::set_max_bullets ( pcstr args )
{
	int new_bullets_count;
	if( XRAY_SSCANF( args, "%d", &new_bullets_count ) == EOF )
		return;

	allocate_bullets_memory			( new_bullets_count );
}

void bullet_manager::allocate_bullets_memory ( u32 new_max_bullets_count )
{
	m_max_bullets_count				= new_max_bullets_count;

	resources::creation_request request	(
		"bullets_memory",
		m_max_bullets_count * ( sizeof( bullet ) + sizeof( bullet* ) ),
		resources::unmanaged_allocation_class
	);

	if( m_bullets_allocator_ref.initialized( ) )
	{
		resources::query_create_resources			(
			&request, 
			1,
			boost::bind( &bullet_manager::bullets_memory_allocated, this, _1 ), 
			g_allocator
		);
	}
	else
	{
		resources::query_create_resources_and_wait	(
			&request, 
			1,
			boost::bind( &bullet_manager::bullets_memory_allocated, this, _1 ), 
			g_allocator
		);
	}
}

void bullet_manager::bullets_memory_allocated ( resources::queries_result& queries )
{
	R_ASSERT						( queries[0].is_successful( ) );
	unmanaged_allocation_resource_ptr new_bullets_memory_ptr = static_cast_resource_ptr<unmanaged_allocation_resource_ptr>( queries[0].get_unmanaged_resource() );
	bullets_type new_bullets_list		( new_bullets_memory_ptr->buffer + m_max_bullets_count * sizeof( bullet ), m_max_bullets_count );

	bool is_realocation = m_bullets_allocator_ref.initialized( ) && m_bullets_allocator_ref->allocated_size( ) > 0;
	if ( is_realocation )
	{
		for ( u32 i = m_bullets_allocator_ref->allocated_size( ) / ( sizeof( bullet ) + sizeof( bullet* ) ); i > m_max_bullets_count; --i )
			displace_one_bullet			( );

		bullets_allocator new_bullets_allocator		= bullets_allocator( new_bullets_memory_ptr->buffer, m_max_bullets_count * sizeof( bullet ) );

		while ( !m_bullets.empty( ) ) {
			bullet*  old_bullet			= m_bullets[0];
			new_bullets_list.push_back	( XRAY_NEW_IMPL( new_bullets_allocator, bullet ) ( *old_bullet ) );
			XRAY_DELETE_IMPL			( *m_bullets_allocator_ref, old_bullet );
		}

		new_bullets_allocator.swap		( *m_bullets_allocator_ref );
	}
	else
	{
		if( m_bullets_allocator_ref.initialized( ) )
			XRAY_DESTROY_REFERENCE		( m_bullets_allocator_ref );

		XRAY_CONSTRUCT_REFERENCE		( m_bullets_allocator_ref, bullets_allocator )( new_bullets_memory_ptr->buffer, m_max_bullets_count * sizeof( bullet ) );
	}
	m_bullets.swap				( new_bullets_list );
	m_bullets_memory_ptr			= new_bullets_memory_ptr;
}

void bullet_manager::request_bullet_decal_material( )
{
	xray::render::material_effects_instance_cook_data* cook_data = 
		NEW(xray::render::material_effects_instance_cook_data)(xray::render::decal_vertex_input_type, NULL, false);

	xray::render::material_effects_instance_cook_data* cook_data_out = 
		NEW(xray::render::material_effects_instance_cook_data)(xray::render::decal_vertex_input_type, NULL, false);
	
	resources::user_data_variant		user_data;
	user_data.set					( cook_data );

	resources::user_data_variant		user_data_out;
	user_data_out.set				( cook_data_out );

	query_resource										(
		"decals/bullet_hit_concrete",
		resources::material_effects_instance_class,
		boost::bind( &bullet_manager::material_ready, this, _1, cook_data ),
		g_allocator,
		&user_data
	);

	query_resource										(
		"decals/bullet_hit_concrete_out",
		resources::material_effects_instance_class,
		boost::bind( &bullet_manager::material_ready_out, this, _1, cook_data_out ),
		g_allocator,
		&user_data_out
	);
}
void bullet_manager::material_ready ( resources::queries_result& data, xray::render::material_effects_instance_cook_data* cook_data )
{
	DELETE											( cook_data );

	if( data.is_successful( ) )
	{
		m_material_ptr								= data[0].get_unmanaged_resource( );
	}
}
void bullet_manager::material_ready_out ( resources::queries_result& data, xray::render::material_effects_instance_cook_data* cook_data )
{
	DELETE											( cook_data );
	
	if( data.is_successful( ) )
	{
		m_material_ptr_out							= data[0].get_unmanaged_resource( );
	}
}

void bullet_manager::emit_bullet ( float3 position, float3 velocity, float air_resistance )
{
	if ( m_bullets_allocator_ref->total_size( ) == m_bullets_allocator_ref->allocated_size( ) )
		displace_one_bullet		( );
	
	bullet* new_bullet			= XRAY_NEW_IMPL	( m_bullets_allocator_ref.c_ptr( ), bullet )( position, velocity, m_game.get_game_world().game_time_sec( ), air_resistance ); 
	m_bullets.push_back	( new_bullet );

#ifndef MASTER_GOLD
	store_bullet_trajectory		( new_bullet );
#endif // #ifndef MASTER_GOLD
}

void bullet_manager::destroy_bullet ( bullets_type::iterator& destroying_bullet_iterator )
{
	bullet*	destroying_bullet			= *destroying_bullet_iterator;							
	m_bullets.erase				( destroying_bullet_iterator );
	XRAY_DELETE_IMPL					( *m_bullets_allocator_ref, destroying_bullet );
	
#ifndef MASTER_GOLD

	bullets_type::iterator	current = m_bullets.begin	( );
	bullets_type::iterator	end		= m_bullets.end	( );

	for ( ; current != end; ++current )
		R_ASSERT( *current != NULL );

#endif // #ifndef MASTER_GOLD

}

void bullet_manager::displace_one_bullet ( )
{
	float3 const&		camera_position			= m_game.get_game_world( ).get_camera_director( )->get_active_camera( )->get_inverted_view_matrix( ).c.xyz( );

#pragma message( XRAY_TODO("jes to all: this may be a bottleneck in some circumstances") )

	float	farthest_distance_square					= 0;
	bullets_type::iterator farthest_bullet	= NULL;

	bullets_type::iterator	current = m_bullets.begin	( );
	bullets_type::iterator	end		= m_bullets.end	( );

	for ( ; current != end; ++current )
	{
		float	distance_square			= ( (*current)->get_position( ) - camera_position ).squared_length( );	
		if( distance_square > farthest_distance_square )
		{
			farthest_bullet				= current;
			farthest_distance_square	= distance_square;
		}
	}
	destroy_bullet			( farthest_bullet );
}

} // namespace stalker2