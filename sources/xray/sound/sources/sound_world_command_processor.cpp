////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_world.h"

namespace xray {
namespace sound {

void sound_world::initialize_command_processor( )
{
	m_xaudio_callback_orders.set_pop_thread_id	( );
	m_xaudio_callback_orders.set_push_thread_id	( );
	m_xaudio_callback_orders.push_null_node		( MT_NEW(sound_command)(&::xray::memory::g_mt_allocator) );

	m_sound_orders_to_delete.set_pop_thread_id	( );
	m_sound_orders_to_delete.set_push_thread_id	( );
	m_sound_orders_to_delete.push_null_node		( NEW(sound_command)(g_allocator) );

	m_end_user_orders.set_pop_thread_id			( );
	m_end_user_orders.push_null_node			( XRAY_NEW_IMPL(m_logic_allocator, sound_command)(m_logic_allocator) );

// logic
	m_logic_orders_to_delete.set_push_thread_id	( );
	m_logic_orders_to_delete.push_null_node		( XRAY_NEW_IMPL(m_logic_allocator, sound_command)(m_logic_allocator) );

	m_logic_callbacks_orders.set_push_thread_id	( );
	m_logic_callbacks_orders.push_null_node		( XRAY_NEW_IMPL(g_allocator, sound_command)(g_allocator) );

// editor
	if ( !m_editor_allocator )
		return;

	m_editor_orders_to_delete.set_push_thread_id( );
	m_editor_orders_to_delete.push_null_node	( XRAY_NEW_IMPL(m_editor_allocator, sound_command)(m_editor_allocator) );

	m_editor_callbacks_orders.set_push_thread_id( );
	m_editor_callbacks_orders.push_null_node	( XRAY_NEW_IMPL(g_allocator, sound_command)(g_allocator) );
}

void sound_world::clear_resources	( )
{
	m_active_items.clear			( );
	process_sound_thread_orders		( );
	// clear editor own commands
	{
		delete_orders( m_xaudio_callback_orders );
		delete_orders( m_sound_orders_to_delete );
		
		m_logic_callbacks_orders.set_pop_thread_id();
		m_editor_callbacks_orders.set_pop_thread_id();
		
		delete_orders( m_logic_callbacks_orders );


		sound_command* cmd			= m_xaudio_callback_orders.pop_null_node( );
		ASSERT(cmd->m_creator == &::xray::memory::g_mt_allocator);
		XRAY_DELETE_IMPL			( cmd->m_creator, cmd );

		m_sound_orders_to_delete.set_pop_thread_id();
		cmd							= m_sound_orders_to_delete.pop_null_node( );
		ASSERT(cmd->m_creator == g_allocator);
		XRAY_DELETE_IMPL			( cmd->m_creator, cmd );

		cmd							= m_logic_callbacks_orders.pop_null_node( );
		ASSERT(cmd->m_creator == g_allocator);
		XRAY_DELETE_IMPL			( cmd->m_creator, cmd );

		if ( m_editor_allocator ) {
			cmd							= m_editor_callbacks_orders.pop_null_node( );
			ASSERT(cmd->m_creator == g_allocator);
			XRAY_DELETE_IMPL			( cmd->m_creator, cmd );
		}

		cmd							= m_end_user_orders.pop_null_node( );
		if(cmd->m_creator==m_logic_allocator)
			m_logic_orders_to_delete.push_back( cmd );
		else
		if(cmd->m_creator==m_editor_allocator)
			m_editor_orders_to_delete.push_back( cmd );
		else
		if(cmd->m_creator==g_allocator)
			XRAY_DELETE_IMPL		( cmd->m_creator, cmd );
		else
			ASSERT(false, "unknown allocator");
	}	

//	m_active_items.clear			( );
//	m_active_streams.clear			( );
}

void sound_world::clear_logic_resources( )
{
	delete_orders( m_logic_orders_to_delete );
	sound_command* cmd			= m_logic_orders_to_delete.pop_null_node( );
	XRAY_DELETE_IMPL			( cmd->m_creator, cmd );
}

void sound_world::clear_editor_resources( )
{
	delete_orders				( m_editor_orders_to_delete );
	sound_command* cmd			= m_editor_orders_to_delete.pop_null_node( );
	XRAY_DELETE_IMPL			( cmd->m_creator, cmd );
}

void sound_world::add_xaudio_callback_command( callback_command* cmd )
{
	m_xaudio_callback_orders.set_push_thread_id	( );
	m_xaudio_callback_orders.push_back			( cmd );
}

void sound_world::add_end_user_command( sound_command* cmd )
{
	m_end_user_orders.set_push_thread_id	( );
	m_end_user_orders.push_back				( cmd );
}

void sound_world::add_logic_callback_command( sound_command* cmd )
{
	m_logic_callbacks_orders.set_push_thread_id	( );
	m_logic_callbacks_orders.push_back			( cmd );
}

void sound_world::add_editor_callback_command( sound_command* cmd )
{
	m_editor_callbacks_orders.set_push_thread_id	( );
	m_editor_callbacks_orders.push_back				( cmd );
}

void sound_world::process_sound_thread_orders( )
{
	// callbacks from xaudio2 core
	while ( !m_xaudio_callback_orders.empty( ) ) 
	{
		sound_command*				to_delete;
		sound_command* const order	= m_xaudio_callback_orders.pop_front( to_delete );
		order->execute				( );
		XRAY_DELETE_IMPL			( to_delete->m_creator, to_delete );
	}

	// callbacks from logic and editor
	while ( !m_end_user_orders.empty( ) ) 
	{
		sound_command*				to_delete;
		sound_command* const order	= m_end_user_orders.pop_front( to_delete );
		order->execute				( );
		
		if(to_delete->m_creator==m_logic_allocator)
			m_logic_orders_to_delete.push_back(to_delete);
		else
		if(to_delete->m_creator==g_allocator)
			m_sound_orders_to_delete.push_back(to_delete);
		else
		if(to_delete->m_creator==m_editor_allocator)
			m_editor_orders_to_delete.push_back(to_delete);
		else
			ASSERT(false, "unknown allocator");
	}

	// delete sound -created orders
	delete_orders( m_sound_orders_to_delete );
}

void sound_world::process_editor_callback_orders( )
{
	m_editor_callbacks_orders.set_pop_thread_id();
	m_editor_orders_to_delete.set_pop_thread_id();

	while ( !m_editor_callbacks_orders.empty( ) ) 
	{
		sound_command*				to_delete;
		sound_command* const order	= m_editor_callbacks_orders.pop_front( to_delete );
		order->execute				( );

		m_sound_orders_to_delete.push_back( to_delete );
	}
	// delete editor -created orders
	delete_orders( m_editor_orders_to_delete );
}

void sound_world::process_logic_callback_orders( )
{
	m_logic_callbacks_orders.set_pop_thread_id();
	m_logic_orders_to_delete.set_pop_thread_id();

	while ( !m_logic_callbacks_orders.empty( ) ) 
	{
		sound_command*				to_delete;
		sound_command* const order	= m_logic_callbacks_orders.pop_front( to_delete );
		order->execute				( );

		m_sound_orders_to_delete.push_back( to_delete );
	}

	// delete logic -created orders
	delete_orders( m_logic_orders_to_delete );
}

} // namespace sound
} // namespace xray


