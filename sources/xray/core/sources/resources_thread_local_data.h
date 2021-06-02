////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURES_THREAD_LOCAL_DATA_H_INCLUDED
#define XRAY_RESOURES_THREAD_LOCAL_DATA_H_INCLUDED

#include "resources_helper.h"
#include <xray/intrusive_list.h>
#include <boost/intrusive/set.hpp>

namespace xray {
namespace resources {

//----------------------------------------------------------
// thread local callbacks
//----------------------------------------------------------

class	fs_task;
class	queries_result;

typedef boost::intrusive::set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >		thread_local_data_tree_member_hook;

class	thread_local_data
{
public:
	inline thread_local_data	(threading::thread_id_type thread_id = 0, memory::base_allocator * allocator = 0) 
	:	dispatching_callbacks										(false), 
		m_allocator													(allocator), 
		in_transaction												(false), 
		m_thread_id													(thread_id), 
		deleting_resources											(0), 
		deallocating_resources										(0), 
		resources_to_deallocate_after_destroy_in_other_thread_count	(0),
		in_translate_query_counter									(0),
		disable_translate_query_counter_check						(0)
	{}

	inline thread_local_data	(thread_local_data const &) { NOT_IMPLEMENTED(); }

	intrusive_list<queries_result, queries_result *, &queries_result::m_next_ready>					finished_queries;
	intrusive_list<fs_task, fs_task *, & fs_task::m_next>									ready_fs_tasks;
	
	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	// intrusive_list<query_result, query_result *, &query_result::m_next_to_create_resource>		to_create_resource;
	intrusive_list<query_result, query_result *, &query_result::m_union_nexts>						to_create_resource;
	// intrusive_list<query_result, query_result *, &query_result::m_next_to_allocate_raw_resource>	to_allocate_raw_resource;
	intrusive_list<query_result, query_result *, &query_result::m_union_nexts>						to_allocate_raw_resource;
	// intrusive_list<query_result, query_result *, &query_result::m_next_to_allocate_resource>		to_allocate_resource;
	intrusive_list<query_result, query_result *, &query_result::m_union_nexts>						to_allocate_resource;
	// intrusive_list<query_result, query_result *, &query_result::m_next_to_translate_query>		to_translate_query;
	intrusive_list<query_result, query_result *, &query_result::m_union_nexts>						to_translate_query;

	intrusive_list<raw_file_buffer_helper, raw_file_buffer_helper *, &raw_file_buffer_helper::next>	to_free_user_buffers;

 	bool																							in_transaction;

	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	// intrusive_list<query_result, query_result *, &query_result::m_next_to_init>					to_init_by_transaction;
	intrusive_list<query_result, query_result *, &query_result::m_union_nexts>						to_init_by_transaction;

	intrusive_list<unmanaged_resource, unmanaged_resource *,&unmanaged_resource::m_next_delay_delete>	delayed_delete_unmanaged_resources;

	intrusive_list<unmanaged_resource_buffer, unmanaged_resource_buffer *,
				   & unmanaged_resource_buffer::m_next_to_deallocate>								delayed_deallocate_unmanaged_resources;

	typedef intrusive_list<query_result, query_result *, &query_result::m_union_nexts> queries_with_tasks_finished_list;
	queries_with_tasks_finished_list		queries_with_tasks_finished;

	threading::atomic32_type	deleting_resources;
	threading::atomic32_type	deallocating_resources;
	threading::atomic32_type	resources_to_deallocate_after_destroy_in_other_thread_count;
	bool						dispatching_callbacks;
	u32							in_translate_query_counter;
	u32							disable_translate_query_counter_check;


	void	delete_this ()
	{ 
		thread_local_data * this_ptr	=	this;
		XRAY_DELETE_IMPL					(m_allocator, this_ptr);
	}

public:
	threading::thread_id_type			thread_id() const { return m_thread_id; }

private:
	memory::base_allocator *			m_allocator;
	threading::thread_id_type			m_thread_id;
public:
	thread_local_data_tree_member_hook	tree_hook; // need to be public
};

typedef boost::intrusive::member_hook
< 
	thread_local_data, 
	thread_local_data_tree_member_hook,
	& thread_local_data::tree_hook 

>	thread_local_data_helper_option;

class thread_local_data_compare
{
public:
	bool	operator () (thread_local_data const & left, thread_local_data const & right) const
	{
		return	left.thread_id() < right.thread_id();
	}
	bool	operator () (threading::thread_id_type const & left, thread_local_data const & right) const
	{
		return	left < right.thread_id();
	}
	bool	operator () (thread_local_data const & left, threading::thread_id_type const & right) const
	{
		return	left.thread_id() < right;
	}
};

typedef	boost::intrusive::set
< 
	thread_local_data, 
	thread_local_data_helper_option, 
	boost::intrusive::compare< thread_local_data_compare >,
	boost::intrusive::constant_time_size<false> 

>	thread_local_data_tree;

} // namespace resources 
} // namespace xray 

#endif // #ifndef XRAY_RESOURES_THREAD_LOCAL_DATA_H_INCLUDED
