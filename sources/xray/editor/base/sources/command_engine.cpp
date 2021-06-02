////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_engine.h"
#include "command_transaction.h"

namespace xray {
namespace editor_base {

command_engine::command_engine		( u32 stack_limit ):
m_stack_pointer				( 0 ),
m_stack_limit				( stack_limit ),
m_count_trans_commands		( 0 ),
m_transaction_started		( false ),
m_revert_transaction_started( false ),
m_start_revert_transaction	( false ),
m_revert_mode				( false ),
m_save_flag					( false ),
m_saved_stack_pointer		( 0 ),
m_preview_command			( nullptr ),
m_preview_failed			( false )
{

}

command_engine::~command_engine			( )
{
	clear_stack	( );
}

bool command_engine::run				( xray::editor_base::command^ command )
{
	ASSERT( m_preview_command == nullptr || m_transaction_started, "Command engine is still in preview mode." );
	return run_impl( command );
}
bool	command_engine::run_preview		( xray::editor_base::command^ command )
{
	ASSERT( !m_preview_command, "Command engine is allready in preview mode." );
	ASSERT( !m_revert_mode );

	m_preview_command	= command;
	m_preview_command->start_preview( );
	m_preview_failed = !run_impl( command );

	return !m_preview_failed;
}

void command_engine::end_preview()
{
	ASSERT( m_preview_command );
	ASSERT( !m_revert_mode );

	if( !m_preview_command->end_preview() )
		m_preview_failed = true;

	if( m_preview_failed )
	{
		pop_command	();
		delete m_preview_command;
	}
	m_preview_command = nullptr;
}

bool command_engine::run_impl			( xray::editor_base::command^ command )
{
	ASSERT( !m_revert_transaction_started, "Not allowed to run a command in undo stage!" );

	bool result;
	if( !m_transaction_started )
	{
		if( !m_revert_mode )
		{
			clear_redo_stack	( );
			push_command		( command );
		}
		else
			++m_stack_pointer;

		m_transaction_commands_count = 0;


		m_transaction_started	= true;
		result					= command->commit( );
		m_transaction_started	= false;

		if( !result )
		{
			if( !m_revert_mode )
			{
				if( m_preview_command != command ) // for preview command command discard will performed on end preview stage.
				{
					pop_command		();
					delete			command;
				}
			}
			else
			{
				ASSERT				( false, "Command failed on redo!" );
			}
		}
		else
		{
			if( m_transaction_commands_count > 0 )
				insert_transaction_command( );

			finalize_old	( );
		}

		return result;
	}

	//////////////////////////////////////////////////////////////////////////
	// This is the case when the command is run form other command 
	// so the command will be added into the parent command's transaction

	ASSERT( m_preview_command == nullptr, "It is not allowed to run nested comand from a preview command. " );
	// Add command to the command stack
	// if the function in rerun mode then command is overwritten 
	// into existing item.
	if( !m_revert_mode )
		push_command( command );
	else
	{
		ASSERT( m_command_stack[m_command_stack.Count+m_stack_pointer] == nullptr );
		m_command_stack[m_command_stack.Count+m_stack_pointer] = command;
		++m_stack_pointer;
	}

	// run the command
	result			= command->commit( );

	// if command succeeds then just increment transaction commands count.
	// if the result is negative than remove the command from the command stack
	if( result )
		++m_transaction_commands_count;
	else
	{
		if( !m_revert_mode )
		{
			if( m_preview_command != command ) // for preview command command discard will performed on end preview stage.
			{
				pop_command();
				delete command;
			}
		}
		else
		{
			--m_stack_pointer;
			//ASSERT( *(m_command_stack.end()+m_stack_pointer) != NULL );
			//DELETE( command, call_destructor_predicate() );
			//*(m_command_stack.end()+m_stack_pointer)= NULL;
			ASSERT( m_command_stack[m_command_stack.Count+m_stack_pointer] != nullptr );
			delete command;
			m_command_stack[m_command_stack.Count+m_stack_pointer] = nullptr;

		}
	}


	return result;
}

int command_engine::rollback			( int steps )
{
	int dest_ptr		= m_stack_pointer+steps;
	if( m_command_stack.Count < -dest_ptr || dest_ptr > 0 || steps == 0 )
		return 0;

	int total_steps			= steps;
	int steps_to_revert		= total_steps;

	if( steps < 0 )
	{
		int idx = m_command_stack.Count + m_stack_pointer - 1;
		for( ; steps_to_revert != 0; --idx, ++steps_to_revert )
		{
			xray::editor_base::command^ cmd			= m_command_stack[idx];
			cmd->rollback			();

			if( m_revert_transaction_started && steps_to_revert != -1 )
			{
				delete				cmd;
				m_command_stack[idx] = nullptr;
			}

			--m_stack_pointer;

			if( m_start_revert_transaction )
			{
				m_start_revert_transaction		= false;

				if( m_command_stack.Count < - (m_stack_pointer + m_count_trans_commands) )
				{
					++m_stack_pointer;
					return total_steps;
				}

				delete				cmd;
				m_command_stack[idx] = nullptr;

				m_revert_transaction_started	= true;

				int tnas_steps_reverted = rollback( m_count_trans_commands );

				total_steps			+= tnas_steps_reverted;
				idx					+= tnas_steps_reverted;

				m_revert_transaction_started = false;
			}
		}
		ASSERT( m_stack_pointer <= dest_ptr);
	}
	else
	{
		total_steps					= steps;
		steps_to_revert				= total_steps;

		int idx = m_command_stack.Count + m_stack_pointer;
		for( ; steps_to_revert != 0; ++idx, --steps_to_revert )
		{
			m_revert_mode		= true;
			run					( m_command_stack[idx] );
			m_revert_mode		= false;
		}

		ASSERT( m_stack_pointer >= dest_ptr);
	}

	return total_steps;
}
void command_engine::clear_stack	( )
{
	clear_redo_stack	( );
	clear_undo_stack	( );
}

void command_engine::call_command_destructor( xray::editor_base::command^ command )
{
	if( command )
		command->~command	( );
}

void command_engine::clear_undo_stack	( )
{
	int idx					= 0;
	int idx_en				= m_command_stack.Count + m_stack_pointer;

	for( ; idx != idx_en; ++idx )
		delete m_command_stack[idx];

	m_command_stack.RemoveRange(0, m_command_stack.Count+m_stack_pointer);
}

void command_engine::clear_redo_stack	( )
{
	int idx					= m_command_stack.Count + m_stack_pointer;
	int idx_en				= m_command_stack.Count;

	for( ; idx != idx_en; ++idx )
		delete m_command_stack[idx];

	m_command_stack.RemoveRange(m_command_stack.Count+m_stack_pointer, -m_stack_pointer);

	if( m_saved_stack_pointer > m_stack_pointer )
		m_save_flag = false;
	else
		m_saved_stack_pointer += (0 - m_stack_pointer);

	m_stack_pointer				= 0;
}

void command_engine::finalize_old		( )
{
	while( (u32)m_command_stack.Count >= m_stack_limit )
	{
		xray::editor_base::command^ comm = m_command_stack[0];
		delete comm;
		m_command_stack.RemoveAt(0);
	}
}

void command_engine::rollback_transaction( int steps )
{
	ASSERT( steps < 0 );

	if( m_revert_transaction_started )
		return;

	m_start_revert_transaction	= true;
	m_count_trans_commands		= steps-1;
}

void command_engine::insert_transaction_command( )
{
	xray::editor_base::command_transaction^ transaction_command = gcnew xray::editor_base::command_transaction( this, m_transaction_commands_count );

	if( !m_revert_mode )
	{
		m_command_stack.Add	( transaction_command );
		--m_saved_stack_pointer;
	}
	else
	{
		ASSERT( m_command_stack[m_command_stack.Count+m_stack_pointer] == nullptr, "Redone command does not correspond to the oreginal execution!" );

		m_command_stack[m_command_stack.Count+m_stack_pointer] = transaction_command;
		++m_stack_pointer;

		ASSERT( m_stack_pointer==0 || m_command_stack[m_command_stack.Count+m_stack_pointer] != nullptr, "Redone command does not correspond to the oreginal execution!" );
	}
}

bool command_engine::preview			( )
{
	ASSERT( m_preview_command != nullptr && !m_revert_mode );
	ASSERT( !m_preview_failed, "The command is failed or canceled. Needed to call end_preview().");

	if( m_preview_failed || !m_preview_command->commit() )
		m_preview_failed = true;

	return !m_preview_failed;
}

void command_engine::set_saved_flag	( )
{
	m_save_flag				= true;
	m_saved_stack_pointer	= m_stack_pointer;
}

bool command_engine::saved_flag		( )
{
	return (m_save_flag && (m_saved_stack_pointer == m_stack_pointer));
}

}// namespace editor_base
}// namespace xray
