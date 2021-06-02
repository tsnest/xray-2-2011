////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ASYNCHRONOUS_DEVICE_H_INCLUDED
#define ASYNCHRONOUS_DEVICE_H_INCLUDED

#include <xray/fs/asynchronous_device_commands.h>
#include <xray/fs/device_file_system_proxy.h>
#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace resources { class resources_manager; } 
namespace vfs {
	struct	mount_root_node_functions;
	class	archive_mounter;
} // namespace vfs

namespace fs_new {

class asynchronous_device_interface : private core::noncopyable
{
public:
	typedef					asynchronous_device_query		query_type;

public:
							asynchronous_device_interface	(synchronous_device_interface * async_interface);
							asynchronous_device_interface	(device_file_system_interface * device, watcher_enabled_bool watcher_enabled);
	inline device_mode_enum	device_mode						() const { return m_device_mode; }

	// should be called from user threads (or as a part of resource manager dispatch_callbacks)
	void					dispatch_callbacks				();
	void					tick							(bool call_from_dedicated_thread);

	void					finalize						();
	void					finalize_thread_usage			();

	// asynchronous commands: return false on out of memory
	bool					query_open_file					(native_path_string	const &			physical_path,
													  		 open_file_params const &			open_file_params,
													  		 query_open_file_callback const &	callback,
															 memory::base_allocator *			allocator,
															 threading::event *					event_to_fire_after_execute);

	bool					query_close_file				(file_type *				file,
															 memory::base_allocator *	allocator,
															 threading::event *			event_to_fire_after_execute);
	bool					query_write_file				(query_write_file_args const & args,
															 memory::base_allocator *	allocator);
	bool					query_read_file					(query_read_file_args const & args,
															 memory::base_allocator *	allocator);
	bool					query_custom_operation			(query_custom_operation_args const & args,
															 memory::base_allocator *	allocator);
	
	bool					query_physical_path_info		(native_path_string const &	native_physical_path,
															 query_physical_path_info_callback const & callback,
															 memory::base_allocator *	allocator,
															 threading::event *			event_to_fire_after_execute);

	void					wakeup_thread					() { m_wakeup_event.set(true); }

private:
	void					get_synchronous_access			(synchronous_device_interface *	in_out_synchronous_interface, 
															 memory::base_allocator *		allocator);

private:
	void					push_query						(query_type * query);
	void					push_high_priority_query		(query_type * query);

private:
	typedef					threads_channel_query_base_helper		< query_type >		query_type_base;

	typedef					intrusive_mpsc_queue					< query_type, query_type_base, & query_type_base::m_next_forward >		
								forward_queue_type;

	typedef					one_way_threads_channel_with_response	< query_type, forward_queue_type, null_device_query >	
								queries_channel;
private:
	void					dispatch_callbacks				(queries_channel * queries);
	void					process_queries					(queries_channel * queries);
	void					process_queries					();
	device_file_system_proxy const &	get_device			() { return m_device; }
	void					set_synchronous_thread_id		(threading::thread_id_type const thread_id);
	threading::thread_id_type	get_synchronous_thread_id	() const { return m_synchronous_thread_id; }

private:
	queries_channel						m_queries;
	queries_channel						m_high_priority_queries;
	threading::atomic32_type			m_processing_queries;
	threading::thread_id_type			m_synchronous_thread_id;

	threading::event					m_wakeup_event;

	device_mode_enum					m_device_mode;
	device_file_system_proxy			m_device;

	friend class						synchronous_device_interface;
	friend class						::xray::resources::resources_manager;
	friend class						synchronize_device_query;

}; // class asynchronous_device_interface

} // namespace fs_new
} // namespace xray

#endif // #ifndef ASYNCHRONOUS_DEVICE_H_INCLUDED