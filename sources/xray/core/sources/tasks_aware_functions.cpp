////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/tasks_aware_functions.h>

namespace xray {
namespace tasks {

void	on_current_thread_locks					();
void	on_current_thread_unlocks				();

void   yield (u32 const ms)
{
	if ( ms != 0 )
		tasks::on_current_thread_locks			();
	threading::yield							(ms);
	if ( ms != 0 )
		tasks::on_current_thread_unlocks		();
}

} // namespace tasks
} // namespace xray
