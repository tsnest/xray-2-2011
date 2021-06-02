////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_delegate.h"

using xray::render::command_delegate;

command_delegate::command_delegate	( Delegate const& delegate ) :
	m_delegate	( delegate )
{
	ASSERT		( delegate );
}

void command_delegate::execute		( )
{
	m_delegate	( );
}