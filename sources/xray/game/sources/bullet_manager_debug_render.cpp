////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"


#ifndef MASTER_GOLD	


#include "bullet_manager.h"
#include "bullet.h"
#include "game.h"
#include "game_world.h"

#include <xray/buffer_vector.h>
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/debug_renderer.h>

namespace stalker2 {

static void		draw_bullets( game& game, render::debug::renderer& renderer, buffer_vector<bullet*> const& m_bullets );
static void		draw_collision_points(
					game& game,
					render::debug::renderer& renderer,
					debug::vector<float3>& collision_points,
					debug::vector<math::color>& points_colors
				);
static void		draw_main_bullet_path(
					game& game,
					render::debug::renderer& renderer,
					vectora< render::vertex_colored > bullet_trajectories_points,
					debug::vector< u32 > bullet_sequences_sizes
				);
static void		draw_collision_rays( game& game, render::debug::renderer& renderer, stalker2::bullet* bullet );
static void		draw_decal_data( game& game, render::debug::renderer& renderer, debug::vector<bullet_manager::decal_data>& decals );

void			bullet_manager::render_debug				( )
{
	render::debug::renderer& renderer = m_game.renderer( ).debug( );

	draw_bullets			( m_game, renderer, m_bullets );
	if( m_is_draw_collision_points )
		draw_collision_points	( m_game, renderer, m_collision_points, m_collision_point_colors );

	if( m_is_fixed )
		renderer.draw_frustum( m_game.get_render_scene( ), 0.5f, .1f, 2, 1.333f, m_fixed_position, m_fixed_direction, float3( 0, 1, 0 ), math::color( 0, 0, 255 ) );

	if( m_is_draw_collision_trajectories && !m_bullets.empty( ) )
	{
		buffer_vector<bullet*>::const_iterator	current		= m_bullets.begin( );
		buffer_vector<bullet*>::const_iterator	end			= m_bullets.begin( );

		for( u32 i = 0; current != end; ++i, ++current )
		{
			if( end - current > 8 )
				continue;
			
			draw_collision_rays		( m_game, renderer, *current );
		}
	}

	if( m_is_draw_trajectories )
		draw_main_bullet_path	( m_game, renderer, m_bullet_trajectories_points, m_bullet_sequences_sizes );

	if( m_is_draw_decals_data )
		draw_decal_data			( m_game, renderer, m_decals );
}

static void		draw_bullets( game& game, render::debug::renderer& renderer, buffer_vector<bullet*> const& m_bullets )
{
	buffer_vector<bullet*>::const_iterator	current = m_bullets.begin( );
	buffer_vector<bullet*>::const_iterator	end		= m_bullets.end( );

	for( ; current != end; ++current )
		renderer.draw_sphere_solid( game.get_render_scene( ), (*current)->get_position( ), .1f, math::color( 0, 0, 255, 128 ) ); 
}

static void		draw_collision_points( game& game, render::debug::renderer& renderer, debug::vector<float3>& collision_points, debug::vector<math::color>& points_colors )
{
	vector<float3>::iterator current_position	= collision_points.begin	( );
	vector<float3>::iterator end_position		= collision_points.end		( );
	vector<math::color>::iterator current_color	= points_colors.begin		( );
	for( ; current_position != end_position; ++current_position, ++current_color )
		renderer.draw_sphere_solid( game.get_render_scene( ), *current_position, .05f, *current_color ); 
}

static void		draw_main_bullet_path( 
					game& game,
					render::debug::renderer& renderer,
					vectora< render::vertex_colored > bullet_trajectories_points,
					debug::vector< u32 > bullet_sequences_sizes
				)
{
	u32 sequences_count			= bullet_sequences_sizes.size( );

	if( sequences_count == 0 )
		return;

	math::color	path_color		= math::color( 255, 0, 255 );
	math::color	fall_down_color	= math::color( 0, 0, 255 );

	vectora< render::vertex_colored >::const_iterator start_point	= bullet_trajectories_points.begin( ); 
	vectora< render::vertex_colored >::const_iterator end_point		= start_point;

	for( u32 i = 0 ; i < sequences_count; ++i )
	{
		u16 vertex_count = (u16)bullet_sequences_sizes[i];

		start_point = end_point;
		end_point	= start_point + vertex_count;

		render::debug_indices_type		indices		( g_allocator );

		for( u16 i = 1 ; i < vertex_count; ++i )
		{
			indices.push_back		( i - 1 );
			indices.push_back		( i );
		}

		renderer.draw_lines		( game.get_render_scene( ), start_point, end_point, indices );
	}
}

static void		draw_collision_rays( game& game, render::debug::renderer& renderer, stalker2::bullet* bullet )
{
	render::debug_vertices_type		vertices	( g_allocator );
	render::debug_indices_type		indices		( g_allocator );
	
	math::color	even_color		= math::color( 255, 0, 0 );
	math::color	odd_color		= math::color( 0, 255, 0 );

	bool	is_even				= false;
	float	tmp;

	float3 const& gravity		= game.get_game_world( ).get_bullet_manager( ).get_gravity( );
	float3	last_position		= bullet->evaluate_position( 0, tmp, gravity );
	float	time_step			= 1.0f;
	u16		index				= 0;

	for( u16 i = 1; i * time_step <= 2; ++i ) 
	{

		float	low_time					= ( i - 1 ) * time_step;
		float	high_time					= i * time_step;

		for ( ;; )
		{
			if( low_time == high_time )
				break;

			float	time					= bullet->pick_next_permissible_time( low_time, high_time, gravity );
			if ( time == low_time )
				break;

			float3 position			= bullet->evaluate_position( time, tmp, gravity );
			if( is_even )
			{
				vertices.push_back		( render::vertex_colored( last_position, even_color ) );
				vertices.push_back		( render::vertex_colored( position, even_color ) );
			}
			else
			{
				vertices.push_back		( render::vertex_colored( last_position, odd_color ) );
				vertices.push_back		( render::vertex_colored( position, odd_color ) );
			}

			indices.push_back		( index++ );
			indices.push_back		( index++ );

			last_position			= position;

			is_even					= !is_even;
			low_time				= time;
		}
	}

	renderer.draw_lines		( game.get_render_scene( ), vertices, indices );
}

static void		draw_decal_data( game& game, render::debug::renderer& renderer, debug::vector<bullet_manager::decal_data>& decals )
{
	int count = decals.size( );

	if( count == 0 )
		return;

	render::debug_vertices_type		vertices	( g_allocator );
	render::debug_indices_type		indices		( g_allocator );
	
	math::color	color		= math::color( 255, 0, 0 );
	
	for( u16 i = 0; i < count; ++i )
	{
		bullet_manager::decal_data const& data = decals[i];

		vertices.push_back( render::vertex_colored( data.position + data.normal * data.depth * .5f , color ) );
		vertices.push_back( render::vertex_colored( data.position - data.normal * data.depth * .5f , color ) );

		indices.push_back( i * 2 );
		indices.push_back( i * 2 + 1 );
	}

	renderer.draw_lines		( game.get_render_scene( ), vertices, indices );
}

} // namespace stalker2


#endif // #ifndef MASTER_GOLD