////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_transaction.h"
#include "command_engine.h"

namespace xray {
namespace editor_base {

command_transaction::command_transaction	( command_engine^ engine, int transaction_size ) 
:m_engine				( engine ),
m_transaction_size		( transaction_size )

{

}
bool command_transaction::commit		( )
{
	return true;
}

void command_transaction::rollback		( )
{
	m_engine->rollback_transaction( -m_transaction_size );
}

} // namespace editor_base
} // namespace xray

