////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_mutex.h>

namespace xray {
namespace tasks {

u32		spin_count_before_notify_task_system	();
void	on_current_thread_locks					();
void	on_current_thread_unlocks				();

} // namespace tasks

namespace threading {

void mutex::lock	( ) const
{
	for ( u32 i=0; i<tasks::spin_count_before_notify_task_system(); ++i )
		if ( m_mutex.try_lock() )
			return;

	tasks::on_current_thread_locks	();
	m_mutex.lock					();
	tasks::on_current_thread_unlocks();
}

void mutex::unlock	( ) const
{
	m_mutex.unlock					();
}

bool mutex::try_lock( ) const
{
	return								m_mutex.try_lock( );
}

} // namespace threading
} // namespace xray