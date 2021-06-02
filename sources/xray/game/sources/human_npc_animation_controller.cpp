////////////////////////////////////////////////////////////////////////////
//	Created		: 08.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "human_npc.h"
#include <xray/animation/mixing_math.h>
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/fermi_interpolator.h>
#include <xray/ai/world.h>
#include <xray/ai/movement_target.h>
#include "human_npc_animation_controller_planner.h"
#include "game.h"
#include "game_world.h"
#include <xray/ai_navigation/world.h>
#include "object_scene.h"

using xray::animation::mixing::animation_lexeme;
using xray::animation::mixing::animation_lexeme_parameters;

namespace stalker2 {

static float const agent_radius	= .35f;

void human_npc::setup_animations_controller	( )
{
#ifndef MASTER_GOLD
	animation_space_graph::animation_pair_type const animations[] =
	{
		std::make_pair			( m_walk_forward_arc_left_animation, "left arc" ),
		std::make_pair			( m_walk_forward_arc_right_animation, "right arc" ),
	};

	m_animation_space_graph		= NEW( animation_space_graph )( m_game_world.get_game().ai_navigation_world(), agent_radius, m_walk_forward_animation, animations, 5 );
	m_search_service			= NEW( search_service )();
	m_target_vertex				= NEW( animation_space_vertex_id )();

	m_target_vertex->rotation	= math::quaternion( float3( 0.f, 0.f, 0.f ) );
	m_target_vertex->translation = m_transform.c.xyz();
	m_game_world.get_game().movement_target() = m_target_vertex->translation;
#endif // #ifndef MASTER_GOLD

	m_model_instance->m_animation_player->subscribe	(
		xray::animation::channel_id_on_animation_interval_end,
		boost::bind( &human_npc::on_animation_interval_end, this, _1, _2, _3, _4 ),
		0
	);

	m_next_key_point			= u32(-1);
}

animation::callback_return_type_enum human_npc::on_animation_interval_end(
		animation::skeleton_animation_ptr const& ended_animation,
		pcstr const subscribed_channel,
		u32 const callback_time_in_ms,
		u32 const domain_data
	)
{
	XRAY_UNREFERENCED_PARAMETERS( ended_animation, subscribed_channel, domain_data );
	setup_animations			( callback_time_in_ms );
	return						animation::callback_return_type_call_me_again;
}

void human_npc::setup_animations	( u32 const current_time_in_ms )
{
	if ( !m_current_movement_target )
		return;

	m_model_instance->m_animation_player->tick( current_time_in_ms );
	
	mutable_buffer buffer			( ALLOCA( xray::animation::animation_player::stack_buffer_size ), xray::animation::animation_player::stack_buffer_size );

	R_ASSERT						( m_animation_space_graph );
	R_ASSERT						( m_search_service );

#ifndef MASTER_GOLD
	//m_target_vertex->rotation		= math::quaternion( float3( 0.f, 0.f, 0.f ) );

	float3 const& direction			= m_current_movement_target->direction;
	float const angle_over_x		= math::atan2( direction.y, direction.z );
	float const angle_over_y		= math::atan2( direction.z, direction.x );
	m_target_vertex->rotation		= math::quaternion( math::create_rotation_y( angle_over_y ) * math::create_rotation_x( angle_over_x )  );

	float3 const& movement_position	= m_current_movement_target->target_position;
	if ( !m_target_vertex->translation.is_similar( movement_position ) )
	{
		m_target_vertex->translation = movement_position;
		u32 const current_node_id	= m_game_world.get_game().ai_navigation_world().get_node_id_at( m_transform.c.xyz() );
		u32 const target_node_id	= m_game_world.get_game().ai_navigation_world().get_node_id_at( m_target_vertex->translation );
		if ( current_node_id == u32(-1) || target_node_id == u32(-1) ) 
			m_next_key_point		= u32(-1);
		else
		{
			bool const success		= m_game_world.get_game().ai_navigation_world().find_path(
																						current_node_id,
																						m_transform.c.xyz(),
																						target_node_id,
																						m_target_vertex->translation,
																						agent_radius,
																						m_navigation_path
																					);
			if ( success && !m_navigation_path.empty() )
				m_next_key_point	= 1;
			else
				m_next_key_point	= u32(-1);
		}
	}

	float3							target_position;
	
	if ( m_next_key_point == u32(-1) )
		target_position				= m_transform.c.xyz();
	else
	{
		R_ASSERT					( m_next_key_point );
		float3 const& npc_position	= m_transform.c.xyz( );
		//u32 const next_key_point	= m_next_key_point;
		for ( u32 i = m_next_key_point, n = m_navigation_path.size(); i < n; ++i, ++m_next_key_point )
		{
			float3 const& previous_key_point			= m_navigation_path[m_next_key_point - 1];
			float3 const& current_key_point				= m_navigation_path[m_next_key_point];
			float3 const& previous_to_current			= current_key_point - previous_key_point;
			float const previous_to_current_length		= previous_to_current.length();
			float3 const& previous_to_current_direction	= previous_to_current / previous_to_current_length;
			if ( ( ( npc_position - previous_key_point ) | previous_to_current_direction ) < previous_to_current_length )
			{
				if ( length( npc_position - current_key_point ) <= .3f )
					continue;

				break;
			}
		}
// 		if ( next_key_point != m_next_key_point )
// 			LOG_INFO			( "next key point changed [%d] => [%d]", next_key_point, m_next_key_point );

		if ( m_next_key_point >= m_navigation_path.size() )
		{
//			LOG_INFO			( "path is over!" );
			m_next_key_point	= u32(-1);
			target_position		= m_transform.c.xyz();
		}
		else
			target_position		= m_navigation_path[m_next_key_point];
	}

	m_target_vertex->translation = target_position;

	animation_space_vertex_id	start_vertex_id;
	start_vertex_id.rotation	= math::quaternion( m_transform.get_angles_xyz() );
	start_vertex_id.translation	= m_transform.c.xyz( );

	search_service::path_type	path;
	if ( !m_search_service->search( *m_animation_space_graph, &path, start_vertex_id, *m_target_vertex ) || path.empty() )
	{
		while ( m_next_key_point < m_navigation_path.size() - 1 )
		{
			++m_next_key_point;
			m_target_vertex->translation = target_position;
			if ( !m_search_service->search( *m_animation_space_graph, &path, start_vertex_id, *m_target_vertex ) || path.empty() )
				continue;

			break;
		}

		if ( m_next_key_point >= m_navigation_path.size() - 1 )
		{
			m_target_vertex->translation = movement_position; //m_game_world.get_game().movement_target();
			m_current_movement_target = 0;
			animation_lexeme lexeme( animation_lexeme_parameters( buffer, "idle", m_idle_animation ).weight_interpolator( animation::linear_interpolator( .25f ) ) );
			m_model_instance->m_animation_player->set_target_and_tick( lexeme, current_time_in_ms );

			if ( get_current_behaviour() )
				get_current_behaviour()->get_parent_job().get_event_manager()->emit_event( game_event( "on_location_reached" ) , this );


			return;
		}
// 		else
// 			LOG_INFO			( "next key point advanced successfully :)" );
	}

	m_target_vertex->translation = movement_position; // m_game_world.get_game().movement_target();

	R_ASSERT					( !path.empty() );

	xray::animation::mixing::animation_lexeme left_animation(
		xray::animation::mixing::animation_lexeme_parameters(
			buffer,
			m_animation_space_graph->animations()[m_animation_space_graph->edges()[path.front()].first_animation_index].second,
			m_animation_space_graph->animations()[m_animation_space_graph->edges()[path.front()].first_animation_index].first
		)
		.time_scale( 1.f )
		.time_scale_interpolator( animation::linear_interpolator( .25f ) )
		.weight_interpolator( animation::linear_interpolator( .25f ) )
		.synchronization_group_id( 0 )
	);
	xray::animation::mixing::weight_lexeme left_weight(
		buffer,
		m_animation_space_graph->edges()[path.front()].first_animation_weight,
		animation::linear_interpolator( .25f )
	);

	xray::animation::mixing::animation_lexeme right_animation(
		xray::animation::mixing::animation_lexeme_parameters(
			buffer,
			m_animation_space_graph->animations()[m_animation_space_graph->edges()[path.front()].second_animation_index].second,
			m_animation_space_graph->animations()[m_animation_space_graph->edges()[path.front()].second_animation_index].first,
			left_animation
		)
		.synchronization_group_id( 0 )
		.weight_interpolator( animation::linear_interpolator( .25f ) )
	);

	m_model_instance->m_animation_player->set_target_and_tick( left_animation * left_weight + right_animation * ( 1.f - left_weight ), current_time_in_ms );
#endif // #ifndef MASTER_GOLD
}

} // namespace stalker2