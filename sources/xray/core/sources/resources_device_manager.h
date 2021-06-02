////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_DEVICE_MANAGER_H_INCLUDED
#define RESOURCES_DEVICE_MANAGER_H_INCLUDED

#include "resources_helper.h"
#include "xray/threading_mutex.h"
#include <xray/intrusive_list.h>

namespace xray {
namespace resources {

class device_manager
{
protected:
	typedef	intrusive_list< query_result, query_result *, &query_result::m_next_in_device_manager, threading::single_threading_policy >	queries_list;
public:
							device_manager				(u32 sector_size);

	virtual bool			can_handle_query			(pcstr	path)	=	0;
	void					update						();

private:
	void					push_delayed_query  		(query_result *);
	void					push_query					(query_result *);
	virtual void			push_query_impl				(query_result *) = 0;

	virtual void			grab_sorted_queries			(queries_list & out_queries) = 0;

	void					fill_pre_allocated			();
	bool					pre_allocate				(query_result * query);

	bool					process_query				(query_result * query, fs_new::synchronous_device_interface const & device);
	void					on_query_processed			(query_result * query, bool result);

	void					on_pre_allocated_size_change(int size_delta, query_result * query);
	
	int						m_pre_allocated_size;

	threading::mutex		m_pre_allocated_mutex;

	// note:
	// when amount of data for device thread is less then m_min_wanted_pre_allocated_size
	// resource manager thread preallocates more data
	u32						m_min_wanted_pre_allocated_size;

	typedef	associative_vector< u32, u32, vector >	thread_id_to_num_queries;
	thread_id_to_num_queries						m_thread_id_to_num_queries;

	queries_list			m_pre_allocated;

	bool					m_ready;
	void *					m_file_handle;
	int						m_overlapped[(3*sizeof(pvoid) + compile_max<2*sizeof(u32),sizeof(pvoid)>::value)/sizeof(int) ];		// 4*5 == 20 == sizeof(OVERLAPPED)
	u32						m_min_bytes_to_operate;

	queries_list			m_finished;

	bool    				open_file			(fs_new::file_type * *							out_file, 
												 query_result *									query, 
												 fs_new::synchronous_device_interface const &	device);

	bool					do_async_operation	(fs_new::file_type *							file, 
												 query_result *									query,
										  		 fs_new::synchronous_device_interface const &	device,
										  		 mutable_buffer									out_data, 
										  		 file_size_type const							file_pos, 
										  		 bool											sector_aligned);

	bool					process_read_query	(fs_new::file_type *							file,
												 query_result *									query, 
												 fs_new::synchronous_device_interface const &	device);

	bool					process_write_query	(fs_new::file_type *							file,
												 query_result *									query, 
												 fs_new::synchronous_device_interface const &	device);

	bool					m_need_fill_pre_allocated;
	bool					m_doing_async_write;	
	u32						m_sector_size;
	u32						get_sector_size					() { return m_sector_size; }

	enum					{ max_sector_size	=	4096 };
	char					m_sector_data_first[max_sector_size];
	char					m_sector_data_last[max_sector_size];
	file_size_type			m_sector_data_last_file_pos;
	fs_new::native_path_string	m_last_file_name;

	friend	class			query_result;
	friend	class			resources_manager;
};

namespace detail {

typedef  fixed_string<2 * fs_new::max_path_length>	query_logging_string; // contains two pathes

query_logging_string		make_query_logging_string	(query_result * query);

} // namespace detail


} // namespace resources
} // namespace xray

#endif // RESOURCES_DEVICE_MANAGER_H_INCLUDED