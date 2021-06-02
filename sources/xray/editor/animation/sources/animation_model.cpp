////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_model.h"
#include "animation_managed_callback.h"

#pragma managed( push, off )
#	include <xray/animation/api.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/animation/animation_player.h>
#pragma managed( pop )

using xray::animation_editor::animation_model;
using namespace xray::animation;

namespace xray{
namespace animation_editor{

/////////////////////////////////////				I N I T I A L I Z E					///////////////////////////////////////////

animation_model::animation_model(
		xray::render::scene_ptr const& scene,
		xray::render::scene_renderer& scene_renderer,
		xray::render::debug::renderer& debug_renderer,
		System::String^ n,
		System::String^ mn,
		float4x4& m,
		query_result_delegate* callback
	) :
	m_scene				( NEW(render::scene_ptr) (scene) ),
	m_scene_renderer	( scene_renderer ),
	m_debug_renderer	( debug_renderer ),
	m_callback			( callback ),
	m_animation_player	( NEW(animation::animation_player)( ) ),
	m_animation_callback( NEW(animation_managed_callback)( ) )
{
	name				= n;
	model_name			= mn;
	camera_follow_me	= false;
	m_added_to_render	= false;
	m_transform			= NEW(float4x4)( m );

	query_result_delegate* rq	= NEW(query_result_delegate)( gcnew query_result_delegate::Delegate( this, &animation_model::model_loaded ), g_allocator );
	xray::resources::query_resource(
		unmanaged_string( model_name ).c_str( ),
		xray::resources::skeleton_model_instance_class,
		boost::bind( &query_result_delegate::callback, rq, _1 ),
		g_allocator,
		0,
		0,
		assert_on_fail_false
	);

	m_animation_callback->m_delegate = gcnew animation_callback( this, &animation_model::animation_ended );
	m_animation_player->subscribe( animation::channel_id_on_animation_end, boost::bind( &animation_managed_callback::callback, m_animation_callback, _1, _2, _3, _4 ), 0 );
}

animation_model::~animation_model( )
{
	if( m_mixer ) 
		destroy_editor_mixer( *g_allocator, m_mixer );

	m_animation_player->unsubscribe( animation::channel_id_on_animation_end, 0 );
	DELETE				( m_animation_player );
	DELETE				( m_animation_callback );

	if( m_model && m_model->c_ptr( ) )
		DELETE( m_model );

	if( m_callback )
		DELETE( m_callback );

	if( m_transform )
		DELETE( m_transform );

	DELETE( m_scene );

	m_transform		= NULL;
	m_mixer			= NULL;
	m_model			= NULL;
	m_callback		= NULL;
	m_scene			= NULL;
}

/////////////////////////////////////				P R O P E R T I E S					///////////////////////////////////////////

float4x4& animation_model::transform::get ( ) 
{
	return *m_transform;
}
void animation_model::transform::set( float4x4& val )
{
	*m_transform = val;
	if( m_mixer && m_added_to_render )
	{
		m_mixer->set_model_transform	( transform );
		m_scene_renderer.update_model	( *m_scene, (*m_model)->m_render_model, transform );
	}
}

/////////////////////////////////////			P U B L I C   M E T H O D S				///////////////////////////////////////////

xray::render::skeleton_model_ptr animation_model::model( ) 
{
	if( m_model != NULL && m_model->c_ptr( ) != NULL )
		return *m_model;

	return NULL;
}

void animation_model::render( )
{
	if( m_mixer && m_added_to_render )
		m_mixer->render( *m_scene, m_scene_renderer, m_debug_renderer, *m_model );
}

void animation_model::set_target( mixing::expression const& expression, u32 const current_time_in_ms )
{
	if( m_mixer )
		m_mixer->set_target_and_tick( expression, current_time_in_ms );
}

void animation_model::tick( u32 const current_time_in_ms )
{
	if( m_mixer )
		m_mixer->tick( current_time_in_ms );
}

void animation_model::push_expression( mixing::expression const& expression, u32 const expression_time )
{
	if( m_mixer )
	{
		m_mixer->push_expression		( expression );
		m_mixer->push_expression_time	( expression_time );
	}
}

void animation_model::calculate_animations_events( xray::vectora<editor_animations_event>& events )
{
	if( m_mixer )
		m_mixer->calculate_animations_events( events );
}

void animation_model::get_current_anim_states( xray::vectora<editor_animation_state>& result, u32 current_time_in_ms )
{
	if( m_mixer )
		m_mixer->get_current_anim_states( result, current_time_in_ms );
}

camera_follower* animation_model::get_camera_follower( )
{
	if( m_mixer )
		return &( m_mixer->get_camera_follower( ) );

	return NULL;
}

void animation_model::subscribe_footsteps( mixing::animation_lexeme& l )
{
	if( m_mixer )
		m_mixer->subscribe_footsteps( l );
}

void animation_model::reset( )
{
	if( m_mixer )
	{
		m_mixer->reset					( );
		m_mixer->set_model_transform	( transform );
	}
}

void animation_model::add_to_render( )
{
	if( model( ) != NULL && !m_added_to_render )
	{
		m_scene_renderer.add_model		( *m_scene, (*m_model)->m_render_model, transform );
		m_added_to_render				= true;
		transform						= float4x4( ).identity( );
	}
}

void animation_model::remove_from_render( )
{
	if( model( ) != NULL && m_added_to_render )
	{
		m_scene_renderer.remove_model	( *m_scene, (*m_model)->m_render_model );
		m_added_to_render				= false;
	}
}

float4x4 animation_model::get_object_matrix ( ) 
{
	if( m_mixer && m_added_to_render )
		return m_mixer->get_object_matrix_for_camera( );
	else
		return float4x4( ).identity( );
}

/////////////////////////////////////			P R I V A T E   M E T H O D S			///////////////////////////////////////////

void animation_model::model_loaded( xray::resources::queries_result& result )
{
	if( m_scene == NULL )
		return;

	if( !result.is_successful( ) )
		return;

	m_model		= NEW(xray::render::skeleton_model_ptr)( );
	(*m_model)	= static_cast_resource_ptr<xray::render::skeleton_model_ptr>( result[0].get_unmanaged_resource( ) );
	m_mixer		= create_editor_mixer( *g_allocator, *m_animation_player );

	if( m_callback )
		m_callback->callback( result );

	m_callback	= NULL;

	transform	= float4x4( ).identity( ); 
}

void animation_model::animation_ended ( skeleton_animation_ptr const& animation, pcstr channel_id, u32 callback_time_in_ms, u8 domain_data )
{
	animation_end( animation, channel_id, callback_time_in_ms, domain_data );
}

} // namespace animation_editor
} // namespace xray
