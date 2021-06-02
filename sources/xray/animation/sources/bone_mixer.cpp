////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/bone_mixer.h>
#include <xray/animation/animation_layer.h>
#include "mixing_n_ary_tree_animation_node.h"
#include "mixing_animation_interval.h"
#include <xray/render/facade/game_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/render/world.h>
#include <xray/animation/i_editor_mixer.h>
#include <xray/console_command.h>
#include <xray/animation/bone_mixer_data.h>

namespace xray {
namespace animation {

bool g_animation_render_debug = true;

static console_commands::cc_bool animation_render_debug( "animation_render_debug", g_animation_render_debug, true, console_commands::command_type_user_specific );

void shift_bone_mixer_data_to_next_interval( bone_mixer_data& data,  mixing::animation_interval const *intervals, u32 intervals_count, u32 interval_id );

bone_mixer::bone_mixer			( const skeleton &skeleton ):
	m_mix							( skeleton ),
	m_new_pose_delta				( float4x4().identity() ),
	m_current_mix_reference_pose	( float4x4().identity() ),
	m_change_reference_time			( u32(-1) ),
	m_footsteps_position_modifier	( 0 )

#ifdef DEBUG
	,
	m_dbg_data_begin				( 0 ),
	m_dbg_data_end					( 0 )
#endif // #ifdef DEBUG
{
}

bone_mixer::~bone_mixer			( )
{
	if (m_footsteps_position_modifier)
		m_footsteps_position_modifier->set_bone_mixer	( 0 );
}

void bone_mixer::set_footsteps_position_modifier	( footsteps_position_modifier *foot_step_modifier )
{
	m_footsteps_position_modifier	= foot_step_modifier;

	if ( m_footsteps_position_modifier )
		m_footsteps_position_modifier->set_bone_mixer( this );
}

float4x4 shift					( skeleton_animation_ptr const& animation, float const from_time, float const to_time )
{
	return
		animation_layer( animation, 1, 0.f, 1, 0 ).get_bone_displacement(
			0,
			float2 (
				from_time * default_fps,
				to_time * default_fps
			)
		);
}

float4x4 animation_position_shift_until_interval	( mixing::animation_interval const *intervals, u32 intervals_count, u32 from_id,  u32 to_id ) 
{
	R_ASSERT_CMP_U				( from_id, <, intervals_count );
	R_ASSERT_CMP_U				( to_id, <=, intervals_count );
	R_ASSERT					( intervals );

	float4x4 result				= float4x4().identity();
	for ( u32 i = from_id ; i < to_id; ++i )
		result					=
			shift(
				intervals[i].animation().animation(),
				intervals[i].start_time(),
				intervals[i].start_time() + intervals[i].length()
			) * result;
		
	return						result;
}

void bone_mixer::shift_bone_mixer_data_to_next_interval	(
		bone_mixer_data& data,
		mixing::animation_interval const* const intervals,
		u32 const intervals_count,
		u32 const interval_id
	)
{
	
	if ( interval_id == data.m_last_interval_id ) 
		return;
	
	LOG_DEBUG					( "shift_bone_mixer_data_to_next_interval from: %d to %d",  data.m_last_interval_id, interval_id );
	float4x4 shift_before = float4x4().identity();
	if ( data.m_last_interval_id < interval_id ) {
		R_ASSERT_CMP			( data.m_last_interval_id, <, interval_id );
		shift_before			= animation_position_shift_until_interval( intervals, intervals_count, data.m_last_interval_id, interval_id );
	}
	else {
		shift_before			= animation_position_shift_until_interval( intervals, intervals_count, interval_id, data.m_last_interval_id );
		shift_before.try_invert	( shift_before );
	}

	float4x4 start_shift		= shift( intervals[interval_id].animation().animation(), 0,  intervals[interval_id].start_time() );
	data.m_inverted_object_transform_from_animation_start.try_invert	( start_shift ); 
	data.m_object_to_world_when_animation_starts	= shift_before * data.m_object_to_world_when_animation_starts;
	data.m_last_interval_id					= interval_id;
}

void bone_mixer::on_switch_interval						( mixing::n_ary_tree_animation_node const &node, mixing::animation_state& state )
{
	bone_mixer_data& data		= *state.bone_mixer_data;
	s32 lexem_new_cycles_count	= state.new_full_cycles_count; 
	
	if ( !lexem_new_cycles_count )
		return;

	float4x4 const& lexem_cycle_shift	= animation_position_shift_until_interval( node.animation_intervals(), node.animation_intervals_count(), node.start_cycle_animation_interval_id(), node.animation_intervals_count() );
	float4x4 lexem_shift		= float4x4().identity();

	if ( lexem_new_cycles_count > 0 ) {
		for (  ; lexem_new_cycles_count > 0 ; --lexem_new_cycles_count )
			lexem_shift			= lexem_cycle_shift * lexem_shift;
	}
	else {
		for ( ; lexem_new_cycles_count < 0	; ++lexem_new_cycles_count )
			lexem_shift			= lexem_cycle_shift * lexem_shift;

		lexem_shift.try_invert	( lexem_shift );
	}

	ASSERT						( !lexem_new_cycles_count );
	data.m_object_to_world_when_animation_starts	= lexem_shift * data.m_object_to_world_when_animation_starts;
}

void bone_mixer::on_new_animation						(
		mixing::n_ary_tree_animation_node const* const node,
		float const animation_time,
		bone_mixer_data& data,
		u32 const current_time_in_ms
	)
{ 
	if ( (m_change_reference_time != current_time_in_ms) && mix().animations_count() ) {
		m_current_mix_reference_pose	= mix_pose_matrix( );
		m_change_reference_time			= current_time_in_ms;
	}
	
	mix().clear					( );

	data.m_object_to_world_when_animation_starts = m_current_mix_reference_pose;//matrix_to_frame( m_current_mix_reference_pose );
	mixing::animation_interval const& local_current_interval = node->animation_intervals()[node->animation_state().animation_interval_id];
	float4x4 intervals_shift  = animation_position_shift_until_interval( node->animation_intervals(), node->animation_intervals_count(), 0, node->animation_state().animation_interval_id );

	float4x4 animation_time_shift = float4x4().identity();
	if ( animation_time > 0 )
		animation_time_shift = animation_time_shift * shift( local_current_interval.animation().animation(), 0, animation_time );

	float4x4 shift_m = animation_time_shift;
	shift_m.try_invert( float4x4( animation_time_shift * intervals_shift ) );

	data.m_object_to_world_when_animation_starts			= shift_m * data.m_object_to_world_when_animation_starts; 
	data.m_last_interval_id	=  node->animation_state().animation_interval_id;
}

float4x4 bone_mixer::bone_matrix						( bone_index_type const bone_index )const
{
	if ( !m_mix.animations_count( ) )
		return							pose( );

	float4x4 const& bone_matrix			= m_mix.bone_matrix( bone_index, 0 );
//	if ( m_mix.get_skeleton().is_root_bone(bone_index) )
		return							bone_matrix;

//	return								bone_matrix * pose();
}

float4x4	bone_mixer::mix_pose_matrix( )const
{
	float4x4 current_mix_reference_pose_matrix = m_current_mix_reference_pose;

	if ( mix().animations_count( ) == 0 )
		return current_mix_reference_pose_matrix;

	float4x4 bone_disp;
	mix().root_matrix( 0, bone_disp );
	current_mix_reference_pose_matrix = bone_disp * current_mix_reference_pose_matrix;
	current_mix_reference_pose_matrix.c.y = 0;
	return current_mix_reference_pose_matrix;
}

void dump_animation_start( mixing::animation_state const &a, animation_layer const &l )
{
	XRAY_UNREFERENCED_PARAMETERS( &a, &l );
	//LOG_DEBUG("================================================");
	//LOG_DEBUG("	starts : %s ", a.animation->identifier() );
	//LOG_DEBUG("	time : %f ", a.time );
	//LOG_DEBUG("	displacement %f, %f , %f", l.root_start().translation.x, l.root_start().translation.y, l.root_start().translation.z );
	//LOG_DEBUG("================================================");
}

inline float weights_sum( mixing::animation_state const* const data_begin, mixing::animation_state const* const data_end )
{
	float sum_weights = 0.f;
	mixing::animation_state const* i = data_begin;
	for ( ; i != data_end  ; ++i )
		sum_weights += i->weight;
	return sum_weights;
}

void	bone_mixer::update_mix_reference_pose( mixing::animation_state const* const data_begin, mixing::animation_state const* const data_end )
{
	if (  mix().animations_count() == 0 )
			return;

	m_current_mix_reference_pose = mix_pose_matrix( ); // matrix_to_frame( mix_pose_matrix( ) );
	
	for ( mixing::animation_state const* i = data_begin; i != data_end  ; ++i )
	{
		float4x4 start_shift = shift( i->animation, 0,  i->time );
		i->bone_mixer_data->m_inverted_object_transform_from_animation_start.try_invert( start_shift );
		i->bone_mixer_data->m_object_to_world_when_animation_starts = m_current_mix_reference_pose;
	}
	
}

void	bone_mixer::process( mixing::animation_state const* const data_begin, mixing::animation_state const* const data_end )
{ 

#ifdef DEBUG
	m_dbg_data_end  = data_end;
	m_dbg_data_begin = data_begin;
#endif // #ifdef DEBUG

	float sum_weights = weights_sum (data_begin, data_end );

	mix().clear();

	for ( mixing::animation_state const* i = data_begin; i != data_end  ; ++i )
	{
		
		if( i->weight <= 0.f ) // temporaly fix ignore zero weights
			continue;

		animation_layer l( *i );

		l.set_factor( l.factor()/sum_weights ); //temp

		i->bone_mixer_data->m_object_to_world_when_animation_starts = m_new_pose_delta * i->bone_mixer_data->m_object_to_world_when_animation_starts;

		float4x4 mframe = i->bone_mixer_data->m_inverted_object_transform_from_animation_start * i->bone_mixer_data->m_object_to_world_when_animation_starts;

		float4x4 inv_reference_pose; inv_reference_pose.try_invert( m_current_mix_reference_pose );
		float4x4  m = mframe * inv_reference_pose;

		l.set_root_start( m );
		dump_animation_start( *i, l );
		mix().add( l );
		
	}
	m_new_pose_delta = float4x4().identity();

//LOG_DEBUG( "set m_current_mix_reference_pose: %f, %f, %f ", m_current_mix_reference_pose.translation.x, m_current_mix_reference_pose.translation.y, m_current_mix_reference_pose.translation.z );
}

void	bone_mixer::reset_reference_pose( float4x4 const &m )
{
	m_current_mix_reference_pose =  m; // matrix_to_frame( m );
}

void	bone_mixer::set_pose( float4x4 const &m )
{
	m_new_pose_delta = m * invert4x3( m_current_mix_reference_pose );
	m_current_mix_reference_pose =  m; // matrix_to_frame( m );
}

#ifndef	MASTER_GOLD
void	bone_mixer::get_dump_string( string4096 &s ) const
{
	mix().get_dump_string( s, 0 );
}
#endif // #ifndef	MASTER_GOLD

void draw_animation_mix_model( render::scene_ptr const& scene, render::skeleton_model_ptr const& model, render::scene_renderer& renderer, animation_mix const &mix, float time, float4x4 const &object_transform  );

void bone_mixer::render( render::scene_ptr const& scene, render::scene_renderer& renderer, render::debug::renderer& debug_renderer, render::skeleton_model_ptr const& model, bool render_debug_info )
{
	if ( mix().animations_count() == 0 )
		return; // hk. ?

/////////////////////////////render//////////////////////////////////////////
	float4x4 matrix = pose();

	if ( model )
		draw_animation_mix_model( scene, model, renderer, mix(), 0, matrix );
/////////////////////////////////////////////////////////////////////////////	
	
	if ( !g_animation_render_debug || !render_debug_info )
		return;

#ifdef DEBUG
	dbg_render( scene, debug_renderer, m_dbg_data_begin, m_dbg_data_end );
#endif	

	debug_renderer.draw_origin( scene, matrix, .5f );

///////////////////debug///render////////////////////////////////////////////	
	mix().render( scene, debug_renderer, matrix, 0 );
	
	const float3 start =  float3( -0.6f , 2.f, 0 ); //object_moving_bone_matrix.c.xyz() +
//	float3 from = start ;
	const float scale = 0.04f;

	matrix.c.xyz() += start;
	mix().render_animation_time_line( scene, debug_renderer, matrix, 0, scale );
///////////////////debug///render////////////////////////////////////////////	
	if ( m_footsteps_position_modifier )
		m_footsteps_position_modifier->render( scene, debug_renderer );
}

void	bone_mixer::render	( render::scene_ptr const& scene, render::debug::renderer& debug_renderer )
{
	if ( mix().animations_count() == 0 )
		return;

	float4x4 matrix = pose();

#ifdef DEBUG
	dbg_render( scene, debug_renderer, m_dbg_data_begin, m_dbg_data_end );
#endif	

	debug_renderer.draw_origin( scene, matrix, .5f );

///////////////////debug///render////////////////////////////////////////////	
	mix().render( scene, debug_renderer, matrix, 0 );
	
	const float3 start =  float3( -0.6f , 2.f, 0 ); //object_moving_bone_matrix.c.xyz() +
//	float3 from = start ;
	const float scale = 0.04f;

	matrix.c.xyz() += start;
	mix().render_animation_time_line( scene, debug_renderer, matrix, 0, scale );
///////////////////debug///render////////////////////////////////////////////	
	if ( m_footsteps_position_modifier )
		m_footsteps_position_modifier->render( scene, debug_renderer );
}

void	bone_mixer::render( render::scene_ptr const& scene, render::scene_renderer& scene_renderer, render::debug::renderer& debug_renderer, render::static_model_ptr const& model, bone_index_type control_bone )
{
	if (	mix().animations_count() == 0 )
		return; // hk. ?
	
	//float4x4 object_transform;
	//frame_matrix( m_current_mix_reference_pose, object_transform );
	float4x4 object_transform = pose();

	mix().render( scene, debug_renderer, object_transform, 0 );
	if ( m_footsteps_position_modifier )
		m_footsteps_position_modifier->render( scene, debug_renderer );

	if ( model )
		scene_renderer.update_model	( scene, model->m_render_model, mix().bone_matrix( control_bone, 0 ) * object_transform );
}

void	dbg_render( render::scene_ptr const& scene, render::debug::renderer& renderer, mixing::animation_state const &data )
{
	renderer.draw_origin( scene, data.bone_mixer_data->m_object_to_world_when_animation_starts, .5f );
}

void	dbg_render( render::scene_ptr const& scene, render::debug::renderer& renderer, mixing::animation_state const* const data_begin, mixing::animation_state const* const data_end )
{
	for ( mixing::animation_state const* i = data_begin; i != data_end; ++i )
		dbg_render( scene, renderer, *i );
}


} // namespace animation
} // namespace xray