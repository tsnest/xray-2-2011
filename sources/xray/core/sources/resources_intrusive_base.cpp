////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_intrusive_base.h>
#include "resources_helper.h"
#include "resources_manager.h"
#include "resources_vfs_associations.h"

namespace xray {
namespace resources {

//----------------------------------------------------------
// base_of_intrusive_base
//----------------------------------------------------------

bool   base_of_intrusive_base::is_associated_with_fat (managed_resource * const object) const
{
	vfs::vfs_iterator fat_it			=	object->m_fat_it;
	return									is_associated_with(fat_it, object);
}

bool   base_of_intrusive_base::is_associated_with_fat (unmanaged_resource * const object) const
{
	vfs::vfs_iterator fat_it			=	object->m_fat_it;
	return									is_associated_with(fat_it, object);
}

void   base_of_intrusive_base::pin_reference_count_for_safe_destroying ()
{
	threading::interlocked_increment		(m_reference_count); 
	m_flags.set								(flag_pinned_for_safe_destroying);
}

void   base_of_intrusive_base::pin_reference_count_for_query_finished_callback ()	
{ 
	threading::interlocked_increment		(m_reference_count); 
	m_flags.set								(flag_is_pinned_by_query_finished_callback | flag_was_pinned_by_query_finished_callback);
}

void   base_of_intrusive_base::unpin_reference_count_for_query_finished_callback ()
{ 
	R_ASSERT								(m_reference_count > 0); 
	threading::interlocked_decrement		(m_reference_count); 
	m_flags.unset							(flag_is_pinned_by_query_finished_callback);
}

void   base_of_intrusive_base::on_capture_increment_reference_count ()
{ 
	threading::interlocked_increment		(m_reference_count); 
	m_flags.set								(flag_is_captured);
}

void   base_of_intrusive_base::on_release_decrement_reference_count ()
{ 
	R_ASSERT								(m_reference_count > 0); 
	threading::interlocked_decrement		(m_reference_count); 
	m_flags.unset							(flag_is_captured);
}

template <class Resource>
signalling_bool   base_of_intrusive_base::try_unregister_from_fat_or_from_name_registry (Resource * const object, u32 count_that_allows_unregister) const
{
	vfs::vfs_iterator fat_it			=	object->m_fat_it;
	if ( !object->reference_count() )
		LOGI_TRACE							("resources", "zero links to %s", log_string(object).c_str());
	else if ( (u32)object->reference_count() > count_that_allows_unregister )
		return								false;

	pcstr const rare_message			=	"LOL. resource was reclaimed just when we were about to delete it!";
	XRAY_UNREFERENCED_PARAMETER				( rare_message );

	if ( !fat_it.is_end() )
	{
		if ( is_associated_with_fat(object) )
		{
			if ( try_clean_associated(fat_it, count_that_allows_unregister) )
			{
				object->on_deassociated_from_fat	();
			}
			else
			{
				LOG_TRACE					(rare_message);
				return						false;
			}
		}
	}
	
	if ( name_registry_entry * const name_registry_entry =	object->get_name_registry_entry() )
	{
		R_ASSERT_CMP						(name_registry_entry->associated, ==, object);
		
		threading::mutex_raii	raii		(g_resources_manager->name_registry_mutex());

		// count_that_allows_unregister is usually 0
		// value of 1 is needed for game resource manager, 
		// when he tries to de-associate resource, yet not deleting it
		if ( get_reference_count() > (long)count_that_allows_unregister )
		{
			LOG_TRACE						(rare_message);
			return							false;
		}
			
		g_resources_manager->push_name_registry_to_delete	(name_registry_entry);
		object->set_name_registry_entry		(NULL);
	}

	// we ask resource to release its sub-fat / fat_it for unmount to work correctly
	object->clean_sub_fat_and_fat_it		();

	object->set_flags						(resource_flag_is_dying);

	return									true;
}

//----------------------------------------------------------
// managed_intrusive_base
//----------------------------------------------------------

void   managed_intrusive_base::destroy (managed_resource * const resource) const
{
	if ( !try_unregister_from_fat_or_from_name_registry	(resource) )
		return; // resource was reclaimed just when we were about to delete it

#pragma message(XRAY_TODO("calling destroy_resource right in user thread, at this time it seems as sufficient"))
	if ( cook_base * const cook = cook_base::find_cook(resource->get_class_id()) )
	{
		cook->call_destroy_resource						(resource);
	}

	if ( threading::current_thread_id() == g_resources_manager->resources_thread_id() )
		g_resources_manager->free_managed_resource		(resource);
	else
		g_resources_manager->push_delayed_delete_managed_resource	(resource);
}

//----------------------------------------------------------
// unmanaged_intrusive_base
//----------------------------------------------------------

void   unmanaged_intrusive_base::destroy (unmanaged_resource * const resource) const
{
	if ( resource->is_no_delete() )
		return;

	R_ASSERT	(resource->get_deleter_object());

	if ( !try_unregister_from_fat_or_from_name_registry	(resource) )
		return; // resource was reclaimed just when we were about to delete it

 	if ( resource->destruction_thread_id() == threading::current_thread_id() ||
 		 cook_base::destroy_in_any_thread(resource->get_class_id()) )
	{
		g_resources_manager->delete_unmanaged_resource				(resource); // sync deletion
	}
	else
		g_resources_manager->push_delayed_delete_unmanaged_resource	(resource); // async deletion
}

} // namespace resources
} // namespace xray
