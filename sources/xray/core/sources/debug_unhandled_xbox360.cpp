////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_platform.h"

void xray::core::debug::platform::format_message			( )
{
}

void xray::core::debug::platform::save_minidump				( _SYSTEMTIME const& date_time, _EXCEPTION_POINTERS* exception_information )
{
	XRAY_UNREFERENCED_PARAMETER	( date_time );
	XRAY_UNREFERENCED_PARAMETER	( exception_information );
}

void xray::core::debug::platform::prologue_dump_call_stack	( _EXCEPTION_POINTERS* exception_information )
{
	XRAY_UNREFERENCED_PARAMETER	( exception_information );
}

void xray::core::debug::set_thread_stack_guarantee			( )
{
}