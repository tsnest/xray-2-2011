////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_SIMPLE_ASYNCHRONOUS_INTERFACE_H_INCLUDED
#define XRAY_FS_SIMPLE_ASYNCHRONOUS_INTERFACE_H_INCLUDED

#include <xray/threading_active_object.h>
#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>

namespace xray {
namespace fs_new {

struct thread_state
{
	threading::atomic32_type	started;
	threading::atomic32_type	should_exit;
	threading::atomic32_type	exited;

	thread_state	() : started(false), should_exit(false), exited(false) {}
};

class simple_asynchronous_interface 
{
public:
	simple_asynchronous_interface			(watcher_enabled_bool);
	~simple_asynchronous_interface			();

	void	dispatch_callbacks				();

	// order of member is important due to construction
private:
	windows_hdd_file_system					hdd_device;
	windows_hdd_file_system					dvd_device;

public:
	asynchronous_device_interface			hdd_async;
	asynchronous_device_interface			dvd_async;

private:
	thread_state							hdd_thread_state;
	thread_state							dvd_thread_state;

private:
	void	thread_proc						(asynchronous_device_interface *	device, thread_state * state);
	void	start_thread					(asynchronous_device_interface *	device,
											 pcstr 								thread_name, 
											 pcstr 								log_thread_name, 
											 thread_state *						state);

}; // class simple_asynchronous_interface

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_SIMPLE_ASYNCHRONOUS_INTERFACE_H_INCLUDED