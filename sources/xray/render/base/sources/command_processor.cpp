////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_processor.h"
#include <xray/render/base/world.h>

using xray::render::command_processor;
using xray::render::engine::command;
using xray::render::engine::wrapper;
using xray::render::delete_on_tick_callback_type;

command_processor::command_processor	( delete_on_tick_callback_type engine ) :
	delete_on_tick				( engine ),
	m_next_frame_orders_first	( 0 ),
	m_next_frame_orders_last	( 0 ),
	m_command_frame_id			( 0 ),
	m_process_next_frame_orders	( false )
{
	m_orders.set_pop_thread_id	( );
}

void command_processor::initialize_queue( command* command )
{
	m_orders.push_null_node		( command );
}

void command_processor::push_command	( command* const command )
{
	bool const empty			= m_orders.empty();
	m_orders.push_back			( command );
	if ( empty )
		m_wait_form_command_event.set	( true );
}

bool command_processor::run				( bool wait_for_command )
{
	if ( m_process_next_frame_orders ) {
		m_process_next_frame_orders			= false;
		
		command* new_buffer_first			= 0;
		command* new_buffer_last			= 0;

		for ( command* i = m_next_frame_orders_first; i; ) {
			i->execute						( );
			command* order					= i;
			i								= i->next;

			if ( order->remove_frame_id <= m_command_frame_id )
				delete_on_tick				( order );
			else {
				order->next					= 0;

				if ( !new_buffer_first )
					new_buffer_first		= order;
				else
					new_buffer_last->next	= order;

				new_buffer_last				= order;
			}
		}

		m_next_frame_orders_first			= new_buffer_first;
		m_next_frame_orders_last			= new_buffer_last;
	}

	for (u32 i=0; wait_for_command && m_orders.empty() && i<64; ++i )
		threading::yield					( 0 );
	
	if ( wait_for_command && m_orders.empty() )
		m_wait_form_command_event.wait		( 16 );

	while ( !m_orders.empty( ) ) {
		command* to_delete;
		command* const order	= m_orders.pop_front( to_delete );

		u32 const frame_id		= m_command_frame_id;
		order->execute			( );

		if ( order->remove_frame_id <= frame_id )
			delete_on_tick		( to_delete );
		else {
			order->next							= 0;

			if ( !m_next_frame_orders_first )
				m_next_frame_orders_first		= order;
			else
				m_next_frame_orders_last->next	= order;

			m_next_frame_orders_last			= order;
		}

		ASSERT					( frame_id <= m_command_frame_id );
		if ( frame_id < m_command_frame_id ) {
			m_process_next_frame_orders	= true;
			return				( false );
		}
	}

	return						( true );
}

void command_processor::purge_orders	( )
{
	command*					order;
	while ( m_orders.pop_front( order ) )
		delete_on_tick			( order );

	order						= m_orders.pop_null_node();
	delete_on_tick				( order );
}

void command_processor::set_push_thread_id	( )
{
	m_orders.set_push_thread_id	( );
}