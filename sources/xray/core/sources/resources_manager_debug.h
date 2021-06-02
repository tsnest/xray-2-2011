////////////////////////////////////////////////////////////////////////////
//	Created		: 24.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_MANAGER_DEBUG_H_INCLUDED
#define RESOURCES_MANAGER_DEBUG_H_INCLUDED

#include <xray/resources.h>
#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace resources {

class debug_state
{
#if DEBUG_RESOURCES
public:
	typedef intrusive_double_linked_list<query_result_debug, 
										 query_result *, 
										 & query_result_debug::prev_in_pending_query_list, 
										 & query_result_debug::next_in_pending_query_list,
										 threading::mutex,
										 size_policy,
										 no_debug_policy>	pending_query_list;

	typedef intrusive_double_linked_list<query_result_debug, 
										 query_result *, 
										 & query_result_debug::prev_in_vfs_lock_query_list, 
										 & query_result_debug::next_in_vfs_lock_query_list,
										 threading::mutex,
										 size_policy,
										 no_debug_policy>	vfs_lock_query_list;

	typedef intrusive_double_linked_list<unmanaged_resource, 								 
										 unmanaged_resource *,
										 & unmanaged_resource::m_prev_in_global_delay_delete_list, 								 
										 & unmanaged_resource::m_next_in_global_delay_delete_list,								 
										 threading::mutex,
										 size_policy,
										 no_debug_policy>	global_delay_delete_unmanaged_list;								 	

	typedef intrusive_double_linked_list<unmanaged_resource_buffer, 								 
										 unmanaged_resource_buffer *,
										 & unmanaged_resource_buffer::m_prev_in_global_delay_delete_list,
										 & unmanaged_resource_buffer::m_next_in_global_delay_delete_list,
										 threading::mutex,
										 size_policy,
										 no_debug_policy>	global_delay_deallocate_unmanaged_buffer_list;
public:

	void	register_delay_delete_unmanaged		(unmanaged_resource * ptr) { m_global_delay_delete_unmanaged_list.push_back(ptr); }
	void	unregister_delay_delete_unmanaged	(unmanaged_resource * ptr) { m_global_delay_delete_unmanaged_list.erase(ptr); }

	void	register_delay_deallocate_unmanaged_buffer		(unmanaged_resource_buffer * ptr) { m_global_delay_deallocate_unmanaged_buffer_list.push_back	(ptr); }
	void	unregister_delay_deallocate_unmanaged_buffer	(unmanaged_resource_buffer * ptr) { m_global_delay_deallocate_unmanaged_buffer_list.erase		(ptr); }

	void	register_pending_query		(query_result * query) { m_pending_query_list.push_back(query); }
	void	unregister_pending_query	(query_result * query) { m_pending_query_list.erase(query); }
			 
	void	register_vfs_lock_query		(query_result * query) { m_vfs_lock_query_list.push_back(query); }
	void	unregister_vfs_lock_query	(query_result * query) { m_vfs_lock_query_list.erase(query); }

	void	unregister_unmanaged_resource	(unmanaged_resource * const res) { m_unmanaged_resources.erase(res); }
	void	register_unmanaged_resource		(unmanaged_resource * const res) { m_unmanaged_resources.push_back(res); }

	u32							unmanaged_resources_count	() const { return m_unmanaged_resources.size(); }
	unmanaged_resource *		pop_unmanaged_resources		() { return m_unmanaged_resources.pop_all_and_clear(); }
	unmanaged_resource *		next_unmanaged_resource		(unmanaged_resource * resource) const { return m_unmanaged_resources.get_next_of_object(resource); }

	void	fill_stats						(strings::text_tree_item & stats);


private:
	pending_query_list							m_pending_query_list;
	vfs_lock_query_list							m_vfs_lock_query_list;
	global_delay_delete_unmanaged_list			m_global_delay_delete_unmanaged_list;	
	global_delay_deallocate_unmanaged_buffer_list	m_global_delay_deallocate_unmanaged_buffer_list;

	intrusive_double_linked_list<unmanaged_resource, 
								 unmanaged_resource *, 
								 & unmanaged_resource::m_prev_in_global_list, 
								 & unmanaged_resource::m_next_in_global_list,
								 threading::mutex,
								 size_policy,
								 no_debug_policy>	m_unmanaged_resources;

#else // #if DEBUG_RESOURCES
public:
	void	register_delay_delete_unmanaged		(unmanaged_resource * ) { }
	void	unregister_delay_delete_unmanaged	(unmanaged_resource * ) { }

	void	register_delay_deallocate_unmanaged_buffer		(unmanaged_resource_buffer * ) { }
	void	unregister_delay_deallocate_unmanaged_buffer	(unmanaged_resource_buffer * ) { }

	void	register_pending_query			(query_result * ) { }
	void	unregister_pending_query		(query_result * ) { }
			 
	void	register_vfs_lock_query			(query_result * ) { }
	void	unregister_vfs_lock_query		(query_result * ) { }

	void	unregister_unmanaged_resource	(unmanaged_resource * ) { }
	void	register_unmanaged_resource		(unmanaged_resource * ) { }

	u32							unmanaged_resources_count	() const { return 0; }
	unmanaged_resource *		pop_unmanaged_resources		() { return NULL; }
	unmanaged_resource *		next_unmanaged_resource		(unmanaged_resource * ) const { return NULL; }

	void	fill_stats						(strings::text_tree_item & ) {}

#endif // #if DEBUG_RESOURCES

};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_MANAGER_DEBUG_H_INCLUDED