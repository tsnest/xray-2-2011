////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/construction.h>
#include "globals.h"

namespace xray {
namespace logging {

static char		s_globals_holder		[sizeof(globals_struct)];
static threading::atomic32_type	s_globals_creation_flag	=	0;

globals_struct *		globals			=	NULL;

globals_struct::globals_struct	()
{
	initiator_tree						=	NULL;
	tree_allocator						=	NULL;
	log_file_usage						=	xray::logging::uninitialized_log;
}

struct globals_constructor
{
	void operator ()					(globals_struct * globals)
	{
		new (globals)						globals_struct;
	}
};

void	preinitialize_globals			()
{
	bind_pointer_to_buffer_mt_safe			(globals, s_globals_holder, s_globals_creation_flag, globals_constructor());
}

void    finalize_globals				()
{
	globals->~globals_struct				();
}

} // namespace logging
} // namespace xray