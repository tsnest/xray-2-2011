////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_resource_base.h>
#include <xray/resources_managed_resource.h>
#include <xray/resources_unmanaged_resource.h>
#include "resources_allocators.h"

//#include "game_resman_test_resource.h"
#include "game_resman.h"

namespace xray {
namespace resources {

//----------------------------------------------------------
// resource_base
//----------------------------------------------------------

resource_base::resource_base(resource_flags_enum flags, class_id_enum class_id, u32 quality_levels_count) 
:	resource_quality						(class_id, quality_levels_count, flags), 
	m_creation_source						(creation_source_unset), 
	m_name_registry_entry					(NULL),
	m_next_for_query_finished_callback		(NULL),
	m_destruction_observer					(NULL),
	m_construct_thread_id					(threading::current_thread_id()),
	m_memory_type_data						(NULL),
	m_next_for_grm_observer_list			(NULL),
	m_next_in_memory_type				(NULL),
	m_prev_in_memory_type				(NULL)
{
	m_next_for_release_from_grm				= NULL;
}

resource_base::~resource_base ()
{
	unlink_from_parents						();
	// note that unlink_from_children is called from (un)managed_resource destructor
	// to have access to resource info to better log
	R_ASSERT								(m_children_resources.empty());
}

void   resource_base::set_need_cook ()
{ 
	set_flags								(resource_flag_needs_cook);
}		

fs_new::virtual_path_string   resource_base::reusable_request_name ()
{
	vfs::vfs_iterator	fat_it;
	bool associated_with_fat			=	false;
	if ( managed_resource * const managed_resource = cast_managed_resource() )
	{
		fat_it							=	managed_resource->get_fat_it();
		if ( !fat_it.is_end() )
			associated_with_fat			=	managed_resource->is_associated_with_fat(managed_resource);
	}
	else if ( unmanaged_resource * const unmanaged_resource = cast_unmanaged_resource() )
	{
		fat_it							=	unmanaged_resource->get_fat_it();
		if ( !fat_it.is_end() )
			associated_with_fat			=	unmanaged_resource->is_associated_with_fat(unmanaged_resource);
	}
	else
		NOT_IMPLEMENTED						(return	"");

	if ( !fat_it.is_end() )
	{
		if ( associated_with_fat )
			return							fat_it.get_virtual_path();
	}
	else if ( m_name_registry_entry )
		return								m_name_registry_entry->name;

	return									"";
}

u32   resource_base::reference_count () const
{
	return	((resource_base *)(this))->cast_base_of_intrusive_base()->get_reference_count();
}

void   resource_base::on_deassociated_from_fat ()
{
	if ( managed_resource * managed = cast_managed_resource() )
		managed->late_set_fat_it			(vfs::vfs_iterator::end());
	else if ( unmanaged_resource * unmanaged = cast_unmanaged_resource() )
		unmanaged->late_set_fat_it			(vfs::vfs_iterator::end());
	else
		NOT_IMPLEMENTED						();

	set_flags								(resource_flag_deassociated_from_fat_it);
}

bool   resource_base::try_unregister_from_fat_or_from_name_registry (u32 count_that_allows_unregister)
{
	if ( managed_resource * managed_resource = cast_managed_resource() )
	{
		return	cast_base_of_intrusive_base()->try_unregister_from_fat_or_from_name_registry(managed_resource, count_that_allows_unregister);
	}
	else if ( unmanaged_resource * unmanaged_resource = cast_unmanaged_resource() )
	{
		return	cast_base_of_intrusive_base()->try_unregister_from_fat_or_from_name_registry(unmanaged_resource, count_that_allows_unregister);
	}
	NOT_IMPLEMENTED							(return true);
}

bool   resource_base::needs_cook () 
{ 
	return									has_flags(resource_flag_needs_cook); 
}

resource_base::creation_source_enum   resource_base::creation_source () const 
{ 
	return									m_creation_source; 
}

void   resource_base::set_name_registry_entry (name_registry_entry * entry) 
{ 
	m_name_registry_entry				=	entry; 
}

name_registry_entry *   resource_base::get_name_registry_entry () const 
{ 
	return									m_name_registry_entry; 
}

void   resource_base::set_memory_usage (memory_usage_type const & usage) 
{ 
	m_memory_usage_self					=	usage; 
}

void   resource_base::set_destruction_observer (query_result * const query) 
{ 
	m_destruction_observer				=	query; 
}

query_result *   resource_base::get_destruction_observer () const 
{ 
	return									m_destruction_observer; 
}

threading::thread_id_type   resource_base::destruction_thread_id () const 
{ 
	return									m_construct_thread_id; 
}

bool   resource_base::in_grm_observed_list () const 
{
	return									has_flags(resource_flag_in_grm_observed_list); 
}

void   resource_base::set_in_grm_observed_list () 
{ 
	set_flags								(resource_flag_in_grm_observed_list); 
}

bool   resource_base::has_user_references () const
{
	u32 const grm_reference				=	(cast_base_of_intrusive_base() && 
											 cast_base_of_intrusive_base()->is_captured()) ? 1 : 0;

	u32 const reference_count			=	this->reference_count();
	u32 const parents_count				=	get_parents_count();

	return									(reference_count - grm_reference) > parents_count;
}

void   resource_base::schedule_disable_caching ()
{
	if (	cast_base_of_intrusive_base()->is_captured() ||
			has_flags(resource_flag_in_grm_release) )
			return;

	schedule_release_resource_from_grm		(this);
}

fs_new::virtual_path_string   resource_base::get_virtual_path () const 
{
	return									m_fat_it.get_virtual_path();
}

void   resource_base::set_sub_fat_resource (vfs_sub_fat_resource * const sub_fat)
{
	if ( managed_resource * managed = cast_managed_resource() )
		managed->set_sub_fat_resource		(sub_fat);
	else if ( unmanaged_resource * unmanaged = cast_unmanaged_resource() )
		unmanaged->set_sub_fat_resource		(sub_fat);
	else
		if ( query_result * query = cast_query_result() ) {
			// call on wrong object - should not be called for query_result derived class
			// Call Lain
			UNREACHABLE_CODE				( );
		}
}

void   resource_base::clean_sub_fat_and_fat_it	()
{
	if ( managed_resource * managed = cast_managed_resource() )
	{
		managed->late_set_fat_it			(vfs::vfs_iterator::end());
		managed->set_sub_fat_resource		(NULL);
	}
	else if ( unmanaged_resource * unmanaged = cast_unmanaged_resource() )
	{
		unmanaged->late_set_fat_it			(vfs::vfs_iterator::end());
		unmanaged->set_sub_fat_resource		(NULL);
	}
	else
		if ( query_result * query = cast_query_result() ) {
			// call on wrong object - should not be called for query_result derived class
			// Call Lain
			UNREACHABLE_CODE				( );
		}
}

resource_link *	  resource_link_list_front_no_dying (resource_link_list const & list)
{
	resource_link * const front			=	list.front();
	if ( front && front->resource->has_flags(resource_flag_is_dying) )
		return								resource_link_list_next_no_dying(front);

	return									front;
}

resource_link *	  resource_link_list_next_no_dying (resource_link const * object) 
{
	resource_link * next	=	object->next_link;
	while ( next && next->resource->has_flags(resource_flag_is_dying) )
		next	=	next->next_link;
	
	return			next; 
}

} // namespace resources
} // namespace xray
