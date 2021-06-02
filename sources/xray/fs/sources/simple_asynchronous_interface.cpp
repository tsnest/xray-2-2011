////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/simple_asynchronous_interface.h>

namespace xray {
namespace fs_new {

simple_asynchronous_interface::simple_asynchronous_interface	(watcher_enabled_bool watcher_enabled)
	:	hdd_async	(& hdd_device, watcher_enabled),
		dvd_async	(& dvd_device, watcher_enabled)		
{
	start_thread							(& hdd_async, "hdd", "hdd", & hdd_thread_state);
	start_thread							(& dvd_async, "dvd", "dvd", & dvd_thread_state);
}

simple_asynchronous_interface::~simple_asynchronous_interface ()
{
	threading::interlocked_exchange			(hdd_thread_state.should_exit, true);
	threading::interlocked_exchange			(dvd_thread_state.should_exit, true);

	while ( !hdd_thread_state.exited || !dvd_thread_state.exited )
		threading::yield					();
}

void   simple_asynchronous_interface::start_thread	(asynchronous_device_interface *	device,
													 pcstr 								thread_name, 
													 pcstr 								log_thread_name, 
													 thread_state *						state)
{
	threading::spawn						(boost::bind(& simple_asynchronous_interface::thread_proc, this, device, state), 
											 thread_name, log_thread_name, 0, 0);
	while ( !state->started )
		threading::yield					();
}

void   simple_asynchronous_interface::thread_proc (asynchronous_device_interface * device, thread_state * state)
{
	threading::interlocked_exchange			(state->started, true);

	while ( !state->should_exit )
	{
		device->tick						(true);
	}

	threading::interlocked_exchange			(state->exited, true);
}

void   simple_asynchronous_interface::dispatch_callbacks ()
{
	hdd_async.dispatch_callbacks			();
	dvd_async.dispatch_callbacks			();
}

} // namespace fs_new {
} // namespace xray {