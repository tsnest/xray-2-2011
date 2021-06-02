////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_RESOURCE_BASE_H_INCLUDED
#define XRAY_RESOURCES_RESOURCE_BASE_H_INCLUDED

#include <xray/fs_path_string.h>
#include <xray/resources_callbacks.h>
#include <xray/fs_platform_configuration.h>
#include <xray/resources_resource_quality.h>
#include <xray/resources_resource_ptr.h>
#include <xray/resources_fs_iterator.h>
#include <xray/vfs/iterator.h>

template struct XRAY_CORE_API boost::intrusive::make_set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >;
template class	XRAY_CORE_API boost::intrusive::set_member_hook< boost::intrusive::link_mode<boost::intrusive::auto_unlink> >;
template struct XRAY_CORE_API boost::intrusive::link_mode<boost::intrusive::auto_unlink>;

namespace xray {
namespace resources {

class	unmanaged_intrusive_base;
template < typename object_type, typename base_type >
class	resource_ptr;

class	vfs_sub_fat_resource;
typedef resource_ptr					<vfs_sub_fat_resource, unmanaged_intrusive_base>	vfs_sub_fat_resource_ptr;
typedef	child_resource_ptr				<vfs_sub_fat_resource, unmanaged_intrusive_base>	vfs_sub_fat_child_resource_ptr;

#if !XRAY_PLATFORM_PS3
template class XRAY_CORE_API			child_resource_ptr<	vfs_sub_fat_resource, unmanaged_intrusive_base >;
#endif // #if !XRAY_PLATFORM_PS3

class					queries_result;
typedef					boost::function< void ( queries_result & data ) >	query_callback;
typedef void *			fat_it_wrapper;
class					query_result_for_cook;
class 					query_result;
class					name_registry_entry;
class					cook_base;

class XRAY_CORE_API resource_base : public resource_quality
{
public:
	enum								creation_source_enum	{	creation_source_unset,
																	creation_source_fat, 
																	creation_source_user_data,
																	creation_source_physical_path,
																	creation_source_created_by_user,
																	creation_source_translate_query,
																	creation_source_deallocate_buffer_helper	};

	virtual								~resource_base				();

	fs_new::virtual_path_string			reusable_request_name		();
	memory_usage_type const &			memory_usage				() const { return m_memory_usage_self; }
	u32									get_size					() const { return m_memory_usage_self.size; }
	bool								has_user_references			() const;
	u32									reference_count				() const;

	void								schedule_disable_caching	();
	vfs::vfs_iterator					get_fat_it					() const { return m_fat_it; }
	fs_new::virtual_path_string			get_virtual_path			() const;

protected:
										resource_base				(resource_flags_enum flags, class_id_enum class_id, u32 quality_levels_count = 1);

	void								set_need_cook				();
	bool								needs_cook					();
	creation_source_enum				creation_source				() const;
	void								set_name_registry_entry		(name_registry_entry * entry);
	name_registry_entry *				get_name_registry_entry		() const;
	void								set_memory_usage			(memory_usage_type const & usage);
	void								set_class_id				(class_id_enum class_id) { m_class_id = class_id; }

private:
	// should only be called by query_result
	void								set_destruction_observer	(query_result * const query);
	query_result *						get_destruction_observer	() const;
	threading::thread_id_type			destruction_thread_id		() const;
	bool								in_grm_observed_list		() const;
	void								set_in_grm_observed_list	();
	void								on_deassociated_from_fat	();
	bool								try_unregister_from_fat_or_from_name_registry (u32 count_that_allows_unregister);
	void								set_sub_fat_resource		(vfs_sub_fat_resource * sub_fat);

	// used by game resources manager to free links to sub fat and unmount it
	void								clean_sub_fat_and_fat_it	();

public:
	resource_tree_member_hook			grm_satisfaction_tree_hook;	// need to be public

	resource_base *						m_next_in_memory_type;
	resource_base *						m_prev_in_memory_type;

protected:
	vfs::vfs_iterator					m_fat_it;

	name_registry_entry *				m_name_registry_entry;
	union {
		resource_base *					m_next_for_query_finished_callback;
		resource_base *					m_next_for_release_from_grm;
		resource_base *					m_union_of_nexts;
	};

	resource_base *						m_next_for_grm_observer_list;
	query_result *						m_destruction_observer;
	creation_source_enum				m_creation_source;

private:
	threading::thread_id_type			m_construct_thread_id;
	memory_type *						m_memory_type_data;

 	template < ::xray::platform_pointer_enum pointer_for_fat_size>
	friend	class						::xray::fs::fat_node;
	
	friend	class						query_result;
	friend	class						query_result_for_cook;												  
	friend	class						managed_intrusive_base;
	friend	class						unmanaged_intrusive_base;
	friend	class						base_of_intrusive_base;
	friend	class						resources_manager;
	friend	class						game_resources_manager;

	template < typename object_type, typename base_type >
	friend	class						child_resource_ptr;
	friend	class						resource_tree_compare;
	friend	class						zero_outer_reference_checker;
	friend	class						resource_freeing_functionality;
	friend	class						quality_decreasing_functionality;
	friend	class						resources_to_free_collection;
	friend	class						quality_increase_functionality;
	friend	class						releasing_functionality;
	friend	class						cook_base;
};

log_string_type		log_string			(resource_base * );

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_RESOURCE_BASE_H_INCLUDED