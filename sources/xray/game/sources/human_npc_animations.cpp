////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "human_npc.h"
#include "object_scene.h"
#include <xray/animation/mixing_math.h>
#include <xray/animation/animation_player.h>
#include <xray/console_command.h>
#include <xray/ai/animation_item.h>
#include <xray/ai/world.h>

using xray::animation::linear_interpolator;
using xray::animation::mixing::animation_lexeme;
using xray::animation::mixing::weight_lexeme;
using xray::animation::mixing::expression;
using xray::animation::mixing::animation_lexeme_parameters;

static bool s_is_positive		= true;
static xray::console_commands::cc_bool s_animation_is_positive_time_scale_command( "animation_is_positive_time_scale", s_is_positive, false, xray::console_commands::command_type_engine_internal );

namespace stalker2 {

animation::callback_return_type_enum human_npc::on_animation_end(
		animation::skeleton_animation_ptr const& ended_animation,
		pcstr const subscribed_channel,
		u32 const callback_time_in_ms,
		u32 const domain_data
	)
{
	XRAY_UNREFERENCED_PARAMETERS	( ended_animation, subscribed_channel, callback_time_in_ms, domain_data );
	m_is_current_animation_finished	= true;
	
	if ( m_last_animation_emitted )
	{
		stop_animation_playing_impl	( );

		if ( get_current_behaviour() )
			get_current_behaviour()->get_parent_job().get_event_manager()->emit_event( game_event( "animation_end" ) , this );
	}

	return							animation::callback_return_type_dont_call_me_anymore;
}

void human_npc::play_animation		( animation::animation_expression_emitter_ptr const& emitter )
{
	u32 const current_time_in_ms	= m_ai_world.get_current_time_in_ms();
	
	m_model_instance->m_animation_player->tick					( current_time_in_ms );
	m_model_instance->m_animation_player->set_object_transform	( m_transform );

	mutable_buffer buffer			( ALLOCA( xray::animation::animation_player::stack_buffer_size ), xray::animation::animation_player::stack_buffer_size );
	expression const new_expression	= emitter->emit( buffer, m_last_animation_emitted );
	
	m_model_instance->m_animation_player->subscribe	(
		xray::animation::channel_id_on_animation_end,
		boost::bind( &human_npc::on_animation_end, this, _1, _2, _3, _4 ),
		0
	);
	
	m_model_instance->m_animation_player->set_target_and_tick( new_expression, current_time_in_ms );
	m_transform						= m_model_instance->m_animation_player->get_object_transform();

	render_model					( );
}

void human_npc::play_animation	( ai::animation_item const* const target )
{
	if ( m_current_animation && m_last_animation_emitted )
		m_model_instance->m_animation_player->unsubscribe( xray::animation::channel_id_on_animation_end, 0 );
	
	m_current_animation			= target;
	m_is_current_animation_finished = false;
	animation::animation_expression_emitter_ptr animation_emitter = static_cast_resource_ptr< animation::animation_expression_emitter_ptr >( m_current_animation->animation );
	play_animation				( animation_emitter );
	LOG_INFO					( "%s: playing animation %s", get_name(), m_current_animation->name );
}

void human_npc::stop_animation_playing		( )
{
	m_last_animation_emitted	= true;
	
	if ( !m_current_animation )
		return;

	m_model_instance->m_animation_player->unsubscribe( xray::animation::channel_id_on_animation_end, 0 );

	stop_animation_playing_impl	( );
}

void human_npc::stop_animation_playing_impl	( )
{
	LOG_INFO					( "%s: stop playing animation %s", get_name(), m_current_animation->name );
	m_ai_world.on_animation_finish( m_current_animation, m_brain_unit );
	m_current_animation			= 0;
}

void human_npc::tick_animation_player	( u32 const current_time_in_ms )
{
	m_model_instance->m_animation_player->tick( current_time_in_ms );
	m_transform							= m_model_instance->m_animation_player->get_object_transform();
 	render_model						( );
}

} // namespace stalker2
