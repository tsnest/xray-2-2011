////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/synchronous_device_interface.h>
#include <xray/fs/asynchronous_device_interface.h>

namespace xray {
namespace fs_new {

synchronous_device_interface::synchronous_device_interface (device_file_system_interface * device, 
															watcher_enabled_bool watcher_enabled)
	: m_device(device, watcher_enabled), m_synchronize_query(NULL), m_out_of_memory(false)
{
}

synchronous_device_interface::synchronous_device_interface (asynchronous_device_interface * adi, 
															memory::base_allocator *		allocator) 
	:	m_synchronize_query(NULL), m_out_of_memory(false)
{
	get_synchronous_access					(adi, allocator);
}

synchronous_device_interface::synchronous_device_interface (asynchronous_device_interface * adi, memory::base_allocator * allocator, 
															device_file_system_interface * device, watcher_enabled_bool watcher_enabled)
	:	m_device(device, watcher_enabled), m_synchronize_query(NULL), m_out_of_memory(false)
{
	if ( !device )
		get_synchronous_access				(adi, allocator);
}

void   synchronous_device_interface::get_synchronous_access (asynchronous_device_interface * adi, memory::base_allocator * allocator)
{
	adi->get_synchronous_access				(this, allocator);
	if ( m_synchronize_query )
		m_synchronize_query->wait_synchronization	();
}

void   synchronous_device_interface::initialize_private (device_file_system_proxy const &	device,
														 synchronize_device_query *			synchronize_query)
{
	ASSERT											(!m_synchronize_query);
	m_device									=	device;
	m_synchronize_query							=	synchronize_query;
}

synchronous_device_interface::~synchronous_device_interface ()
{
	if ( m_synchronize_query )
		m_synchronize_query->on_synchronized_ended	();
}

} // namespace fs_new
} // namespace xray
