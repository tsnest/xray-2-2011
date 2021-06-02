////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESOURCES_MANAGER_H_INCLUDED
#define GAME_RESOURCES_MANAGER_H_INCLUDED

#include <xray/resources.h>
#include <xray/resources_callbacks.h>
#include <xray/intrusive_double_linked_list.h>
#include "game_resman_free.h"
#include "game_resman_data.h"

namespace xray {
namespace resources {

class game_resources_manager
{
public:
				~game_resources_manager				();

	//--------------------------------------------------------
	// Scheduling Asynchronous Work
	//--------------------------------------------------------
	void		schedule_release_all_resources		();
	bool		is_scheduled_release_all_resources	() const;
	u64			current_increase_quality_tick		() const;

	//--------------------------------------------------------
	// Callbacks
	//--------------------------------------------------------
	void		query_finished_callback				(resource_base * resource);
	void		out_of_memory_callback				(query_result  * out_of_memory_query);
	void		on_node_unmount						(vfs::vfs_iterator & it);
	void		on_node_hides						(vfs::vfs_iterator & it);
	void		on_unmount_started_callback			();
	void		on_resource_freed_callback			(query_result * destruction_observer, memory_usage_type const & memory_usage, class_id_enum class_id);

	void		add_autoselect_quality_query		(query_result * query);
	void		remove_autoselect_quality_query		(query_result * query);
	void		release_sub_fat						(vfs_sub_fat_resource * resource);

	void		tick								();

	void		release_resource					(resource_base * resource);
	void		dispatch_resources_to_release		();
	void		push_resource_to_release			(resource_base * resource);
	void		capture_resource					(resource_base * resource);

private:
	void		tick_memory_types					();
	void			tick_memory_type				(memory_type * );

	bool		try_free_or_decrease_quality		(query_result * , memory_type * );

	bool		try_reallocate_queue				(memory_type * );
	bool			try_reallocate_queue_front		(memory_type * );

	void		log_resources						(memory_type * );
	void		destroy_memory_type_data			(memory_type * );

	void		add_new_resource_to_increase_quality_tree	(resource_base * resource);
	
	void		dispatch_capture					();
	void			dispatch_capture				(resource_base * resource);
private:
	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	//typedef	intrusive_list<resource_base, resource_base *, & resource_base::m_next_for_query_finished_callback>	resources_to_capture_list;
	typedef	intrusive_list<resource_base, resource_base *, & resource_base::m_union_of_nexts>	resources_to_capture_list;
	resources_to_capture_list						m_resources_to_capture;

	//typedef	intrusive_list<resource_base, resource_base *, & resource_base::m_next_for_release_from_grm>	resources_to_release_list;
	typedef	intrusive_list<resource_base, resource_base *, & resource_base::m_union_of_nexts>	resources_to_release_list;
	resources_to_release_list						m_resources_to_release;

	game_resources_manager_data						m_data;
	friend class									resources_to_free_collection;
}; // class game_resources_manager

extern uninitialized_reference<game_resources_manager>	g_game_resources_manager;

void	initialize_game_resources_manager			();
void	finilize_game_resources_manager				();
void	tick_game_resources_manager					();

void	schedule_release_all_resources				();
void	schedule_release_resource_from_grm			(resource_base * resource);
bool	is_release_all_resources_scheduled			();
void	add_autoselect_quality_query				(query_result * query);

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESOURCES_MANAGER_H_INCLUDED