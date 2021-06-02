////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <fastdelegate/fastdelegate.h>
#include <xray/core/core.h>
#include <xray/debug/log_callback.h>

namespace xray {
namespace logging {

enum verbosity_recursion 
{
	recurse_all	=	0,
	recurse_0	=	256,
};

bool	initialize_console				( );
void	initialize_stdstreams_if_needed		( );
void	finalize_console				( );

struct debug_log_disable_raii
{
	debug_log_disable_raii				()	{	xray::debug::disable_log_callback	();	}
	~debug_log_disable_raii				()	{	xray::debug::enable_log_callback	();	}
};

} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_H_INCLUDED