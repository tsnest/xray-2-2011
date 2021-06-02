////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_mixer.h"
#include <xray/animation/skeleton.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_n_ary_tree_time_scale_calculator.h"

using xray::animation::editor_mixer;
using xray::animation::mixing::n_ary_tree_animation_node;
using xray::animation::mixing::n_ary_tree_base_node;
using xray::animation::mixing::n_ary_tree_time_scale_calculator;

xray::animation::i_editor_mixer::editor_mixer_callback* xray::animation::i_editor_mixer::callback = NULL;

editor_mixer::editor_mixer( xray::memory::base_allocator& allocator, xray::animation::animation_player& animation_player ) :
	m_allocator			( allocator ),
	m_animation_player	( animation_player ),
	m_expressions		( allocator ),
	m_expression_times	( allocator )
{
	//m_bone_mixer = XRAY_NEW_IMPL(m_allocator, bone_mixer)(m_skeleton );
	//m_mixer = XRAY_NEW_IMPL(m_allocator, animation_mixer)(*m_bone_mixer);
	//m_footsteps_position_modifier = XRAY_NEW_IMPL( m_allocator, footsteps_position_modifier )( m_fixed_bones );
	//m_bone_mixer->set_footsteps_position_modifier( m_footsteps_position_modifier );


	m_camera_follower.set_object( this );
}

editor_mixer::~editor_mixer( )
{
	//XRAY_DELETE_IMPL(m_allocator, m_mixer);
	//XRAY_DELETE_IMPL(m_allocator, m_bone_mixer);
	//XRAY_DELETE_IMPL( m_allocator, m_footsteps_position_modifier );


	if( i_editor_mixer::callback )
	{
		XRAY_DELETE_IMPL( m_allocator, i_editor_mixer::callback );
		i_editor_mixer::callback = NULL;
	}
}

void editor_mixer::render( xray::render::scene_ptr const& scene, xray::render::scene_renderer& scene_renderer, xray::render::debug::renderer& debug_renderer, xray::render::skeleton_model_ptr model )
{
	XRAY_UNREFERENCED_PARAMETER			( debug_renderer );

	if ( !m_animation_player.are_there_any_animations() )
		return;

	animation::skeleton_ptr	skeleton	= model->m_skeleton;

	u32 const bone_matrices_count		= skeleton->get_non_root_bones_count( );
	float4x4* const bone_matrices		= static_cast<float4x4*>( ALLOCA( bone_matrices_count*sizeof(float4x4) ) );
	m_animation_player.compute_bones_matrices( *skeleton, bone_matrices, bone_matrices + bone_matrices_count );

	float4x4 const object_transform = m_animation_player.get_object_transform( );
	scene_renderer.update_model			( scene, model->m_render_model, object_transform );
	
	scene_renderer.update_skeleton	(
		model->m_render_model,
		bone_matrices,
		bone_matrices_count
	);

	// Draws crosses in each bone position
	//
	//for( u32 i = 0; i < bone_matrices_count; ++i )
	//	debug_renderer.draw_origin( scene, bone_matrices[i] * object_transform, 0.05f  );
}

xray::float4x4 editor_mixer::get_object_matrix_for_camera( ) const
{
	return	m_animation_player.get_object_transform( );
}

void editor_mixer::set_model_transform( xray::float4x4& m )
{
	m_animation_player.set_object_transform( m );
}

void editor_mixer::reset( )
{
	//XRAY_DELETE_IMPL(m_allocator, m_mixer);
	//XRAY_DELETE_IMPL(m_allocator, m_bone_mixer);
	//m_bone_mixer = XRAY_NEW_IMPL(m_allocator, bone_mixer)(m_skeleton );
	//m_mixer = XRAY_NEW_IMPL(m_allocator, animation_mixer)(*m_bone_mixer);
	//m_footsteps_position_modifier->reset();
	//m_bone_mixer->set_footsteps_position_modifier( m_footsteps_position_modifier );

	m_animation_player.reset	( );
}

void editor_mixer::subscribe_footsteps( xray::animation::mixing::animation_lexeme& anim )
{
	XRAY_UNREFERENCED_PARAMETER	( anim );

	//if( m_footsteps_position_modifier )
	//	m_footsteps_position_modifier->subscribe_steps	( anim );
}

void editor_mixer::set_target_and_tick( xray::animation::mixing::expression const& expression, u32 const current_time_in_ms )
{
	m_animation_player.set_target_and_tick( expression, current_time_in_ms );
}

void editor_mixer::tick( u32 const current_time_in_ms )
{
	m_animation_player.tick( current_time_in_ms );
}

void editor_mixer::push_expression( xray::animation::mixing::expression const& expression )
{
	m_expressions.push_back( expression );
}

void editor_mixer::push_expression_time( u32 const expression_time )
{
	m_expression_times.push_back( expression_time );
}

void editor_mixer::calculate_animations_events( xray::vectora<xray::animation::editor_animations_event>& events )
{
	R_ASSERT( m_expressions.size( ) == m_expression_times.size( ) );
	reset( );
	i_editor_mixer::callback = XRAY_NEW_IMPL( m_allocator, editor_mixer_callback )( boost::bind( &editor_mixer::calculate_animations_events_callback, this, &events, _1 ) );
	vectora<mixing::expression>::const_iterator		expressions_start			= m_expressions.begin( );
	vectora<u32>::const_iterator					expression_times_start		= m_expression_times.begin( );
	u32 time = 0;

	for(; expressions_start != m_expressions.end( ); ++expressions_start, ++expression_times_start )
	{
		m_animation_player.set_target_and_tick( *expressions_start, time );
		time = *expression_times_start;
		m_animation_player.tick( time );
	}
	m_expressions.clear			( );
	m_expression_times.clear	( );
	XRAY_DELETE_IMPL			( m_allocator, i_editor_mixer::callback );
	i_editor_mixer::callback	= NULL;
}

void editor_mixer::calculate_animations_events_callback( xray::vectora<xray::animation::editor_animations_event>* events, xray::animation::editor_animations_event const& e )
{
	events->push_back( e );
}

void editor_mixer::get_current_anim_states( xray::vectora<xray::animation::editor_animation_state>& result, u32 current_time_in_ms )
{
	m_animation_player.tick						( current_time_in_ms );
	m_animation_player.fill_animation_states	( result );
}