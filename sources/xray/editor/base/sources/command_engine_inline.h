////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_ENGINE_INLINE_H_INCLUDED
#define COMMAND_ENGINE_INLINE_H_INCLUDED

namespace xray {
namespace editor_base {

void command_engine::push_command( command^ command )
{
	m_command_stack.Add( command );
	--m_saved_stack_pointer;
}

void command_engine::pop_command( )
{
	m_command_stack.RemoveAt(m_command_stack.Count-1);
	++m_saved_stack_pointer;
}

bool command_engine::stack_empty( )
{
	return m_command_stack.Count == 0;
}

bool command_engine::undo_stack_empty( )
{
	return ( -m_stack_pointer == (int)m_command_stack.Count );
}

bool command_engine::redo_stack_empty( )
{
	return ( m_stack_pointer == 0 );
}

bool command_engine::transaction_started( )
{
	return m_transaction_started || m_revert_transaction_started;
}

} // namespace editor_base
} // namespace xray

#endif // #ifndef COMMAND_ENGINE_INLINE_H_INCLUDED