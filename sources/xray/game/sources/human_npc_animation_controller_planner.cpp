////////////////////////////////////////////////////////////////////////////
//	Created		: 09.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "human_npc_animation_controller_planner.h"
#include <xray/animation/mixing_math.h>

namespace stalker2 {

#ifndef MASTER_GOLD

animation_space_vertex_id get_movement	(
		xray::animation::animation_player& player,
		xray::animation::skeleton_animation_ptr const& left,
		pcstr const left_identifier,
		xray::animation::skeleton_animation_ptr const& right,
		pcstr const right_identifier,
		float const left_weight
	)
{
	player.reset				( );
	player.set_object_transform	( float4x4().identity() );

	u32 const buffer_size		= xray::animation::animation_player::stack_buffer_size;
	xray::mutable_buffer buffer	( ALLOCA(buffer_size), buffer_size );

	xray::animation::mixing::animation_lexeme left_lexeme(
		xray::animation::mixing::animation_lexeme_parameters(
			buffer,
			left_identifier,
			left
		)
		.synchronization_group_id( 0 )
	);
	xray::animation::mixing::weight_lexeme left_weight_lexeme( buffer, left_weight, xray::animation::instant_interpolator() );

	xray::animation::mixing::animation_lexeme right_lexeme(
		xray::animation::mixing::animation_lexeme_parameters(
			buffer,
			right_identifier,
			right,
			left_lexeme
		)
	);

	player.set_target_and_tick	( left_lexeme*left_weight_lexeme + right_lexeme*(1.f - left_weight_lexeme), 0 );

	float const left_animation_length	= xray::animation::cubic_spline_skeleton_animation_pinned( left )->length_in_frames() / xray::animation::default_fps;
	player.tick					( math::floor(left_animation_length*1000.f) );
	float4x4 const& transform	= player.get_object_transform( );

	animation_space_vertex_id	result;
	result.rotation				= math::quaternion( transform.get_angles_xyz() );
	result.translation			= transform.c.xyz();
	return						result;
}

bool search_service::search	(
		animation_space_graph const& graph,
		path_type* path,
		vertex_id_type const start_vertex_id,
		vertex_id_type const target_vertex_id
	)
{
	graph_wrapper_type graph_wrapper			= graph_wrapper_type( graph, start_vertex_id );
	path_constructor_type path_constructor		= path_constructor_type( path );
	path_heuristics_type path_heuristics		= path_heuristics_type( graph, target_vertex_id, graph.step_time(), graph.max_speed() );
	search_restrictor_type search_restrictor	= search_restrictor_type(
		graph,
		start_vertex_id,
		target_vertex_id,
		std::numeric_limits< distance_type >::max( ),
		u32(-1),
		max_vertex_count - graph.edges().size()
	);

	bool const result				=
		algorithm_type::find		(
			m_priority_queue,
			graph_wrapper,
			path_constructor,
			path_heuristics,
			search_restrictor
		);
	if ( result )
		return						true;

	vertex_id_type const& best_vertex_id	= path_heuristics.best_vertex_id( );
	vertex_type& vertex				= m_vertex_manager.visited( best_vertex_id ) ? m_vertex_manager.vertex( best_vertex_id ) : m_priority_queue.vertex( best_vertex_id );
	path_constructor.construct_path	( vertex );
	return							true;
}

#endif // #ifndef MASTER_GOLD

} // namespace stalker2