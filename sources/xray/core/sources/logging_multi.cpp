////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logging.h"

void xray::logging::write_to_stdout				( pcstr format, ... )
{
	XRAY_UNREFERENCED_PARAMETER	( format );
}

bool xray::logging::initialize_console			( )
{
	return						false;
}

void xray::logging::finalize_console			( )
{
}

void xray::logging::initialize_stdout_if_needed ( )
{
}