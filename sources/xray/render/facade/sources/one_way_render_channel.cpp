////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/one_way_render_channel.h>
#include <xray/render/world.h>

using xray::render::one_way_render_channel;

class pop_front_predicate : private boost::noncopyable {
public:
	inline	pop_front_predicate		( u32 const current_frame_id ) :
		m_current_frame_id	( current_frame_id )
	{
	}
	
	inline	bool	operator ( )	( xray::render::base_command* const command ) const
	{
		return				!command->is_deferred_command && (command->remove_frame_id <= m_current_frame_id);
	}

	u32 const	m_current_frame_id;
}; // struct pop_front_predicate

void one_way_render_channel::process_next_frame_commands( )
{
	R_ASSERT								( m_process_next_frame_commands );
	m_process_next_frame_commands			= false;

	delayed_commands_queue_type				new_next_frame_commands_queue;
	pop_front_predicate predicate			( m_current_frame_id );

	while ( base_command* const i = m_next_frame_commands_queue.pop_front() ) {
		if ( i->is_deferred_command ) {
			i->defer_execution				( );
			continue;
		}

		i->execute							( );

		if ( predicate(i) )
			m_channel.user_delete_deffered_value ( i );
		else
			new_next_frame_commands_queue.push_back ( i );
	}

	m_next_frame_commands_queue.swap		( new_next_frame_commands_queue );
}

bool one_way_render_channel::render_process_commands	( bool const wait_for_command_if_queue_is_empty )
{
	if ( m_process_next_frame_commands )
		process_next_frame_commands			( );

	if ( m_channel.user_is_queue_empty() && wait_for_command_if_queue_is_empty ) {
		for ( u32 i = 0; m_channel.user_is_queue_empty() && (i < 64); ++i )
			threading::yield				( 0 );
		
		if ( m_channel.user_is_queue_empty() )
			m_wait_form_command_event.wait	( 16 );
	}

	pop_front_predicate predicate			( m_current_frame_id );

	while ( base_command* const i = m_channel.user_pop_front(predicate) ) {
		if ( i->is_deferred_command ) {
			i->defer_execution			( );
			continue;
		}

		u32 const frame_id					= m_current_frame_id;
		i->execute							( );
		R_ASSERT_CMP						( frame_id, <=, m_current_frame_id );

		if ( !predicate(i) )
			m_next_frame_commands_queue.push_back	( i );

		if ( frame_id < m_current_frame_id ) {
			R_ASSERT						( !m_process_next_frame_commands );
			m_process_next_frame_commands	= !m_next_frame_commands_queue.empty( );
			return							false;
		}
	}

	return									true;
}

void one_way_render_channel::render_on_draw_scene			( xray::render::scene_ptr const& scene, xray::render::scene_view_ptr const& scene_view, bool const use_depth )
{
	for ( xray::render::base_command* i = scene->first_command; i; i = i->deferred_next ) {
		if ( (*i).use_depth == use_depth )
			i->execute						( );
	}

	if ( !use_depth ) {
		for ( xray::render::base_command* i = scene_view->first_command; i; i = i->deferred_next )
			i->execute						( );
	}

	// we will be waiting for upcoming !use_depth call
	if ( use_depth )
		return;

	{
		bool found							= false;
		for ( scene_ptr i = m_scenes; i; i = i->next_scene ) {
			if ( i == scene ) {
				found						= true;
				break;
			}
		}

		if ( !found ) {
			scene->next_scene				= m_scenes;
			m_scenes						= scene;
		}
	}

	{
		bool found							= false;
		for ( scene_view_ptr i = m_scene_views; i; i = i->next_scene_view ) {
			if ( i == scene_view ) {
				found						= true;
				break;
			}
		}

		if ( !found ) {
			scene_view->next_scene_view		= m_scene_views;
			m_scene_views					= scene_view;
		}
	}
}

template < typename Scene_OR_SceneViewType >
void one_way_render_channel::move_commands_from_list		( Scene_OR_SceneViewType const& scene_or_scene_view )
{
	pop_front_predicate predicate			( m_current_frame_id );

	xray::render::base_command* i = scene_or_scene_view->first_command;
	for ( ; i; i = i->deferred_next ) {
		i->is_deferred_command				= false;
		if ( predicate(i) ) {
			m_channel.user_delete_deffered_value ( i );
			continue;
		}

		i->is_deferred_command				= true;
		m_next_frame_commands_queue.push_back ( i );
	}

	scene_or_scene_view->first_command		= 0;
	scene_or_scene_view->last_command		= 0;
}

void one_way_render_channel::render_on_end_frame			( )
{
	for ( scene_ptr i = m_scenes; i; i = i->next_scene )
		move_commands_from_list				( i );

	m_scenes								= 0;

	for ( scene_view_ptr i = m_scene_views; i; i = i->next_scene_view )
		move_commands_from_list				( i );

	m_scene_views							= 0;

	++m_current_frame_id;
}