////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "functor_command.h"

using xray::render::functor_command;

functor_command::functor_command		(
		on_execute_type const& on_execute,
		on_defer_execution_type const& on_defer_execution
	) :
	base_command		( !on_defer_execution.empty() ),
	m_on_execute		( on_execute ),
	m_on_defer_execution( on_defer_execution )
{
	ASSERT				( on_execute );
}

void functor_command::execute			( )
{
	m_on_execute		( );
}

void functor_command::defer_execution	( )
{
	m_on_defer_execution( *this );
}