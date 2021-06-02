////////////////////////////////////////////////////////////////////////////
//	Created		: 31.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_TRANSACTION_H_INCLUDED
#define COMMAND_TRANSACTION_H_INCLUDED

#include "command.h"

namespace xray {
namespace editor_base {

ref class command_engine;

public ref class command_transaction : public command
{
public:
						command_transaction		( command_engine^ engine, int transaction_size );
	virtual				~command_transaction	( )	{};
	virtual bool		commit					( ) override;
	virtual void		rollback				( ) override;

private:
	command_engine^		m_engine;
	int					m_transaction_size;

}; // class command_transaction

} // namespace editor_base
} // namespace xray

#endif // #ifndef COMMAND_TRANSACTION_H_INCLUDED