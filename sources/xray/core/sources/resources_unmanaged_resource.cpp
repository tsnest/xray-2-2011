////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_unmanaged_resource.h>
#include "resources_manager.h"

namespace xray {
namespace resources {

//----------------------------------------------------------
// unmanaged_resource
//----------------------------------------------------------

unmanaged_resource::unmanaged_resource (u32 quality_levels_count) 
: resource_base(resource_flag_is_unmanaged_resource, unknown_data_class, quality_levels_count)
{
	constructor_impl						();
}

unmanaged_resource::unmanaged_resource (resource_flags_enum flags, u32 quality_levels_count) 
: resource_base(resource_flags_enum(resource_flag_is_unmanaged_resource | flags), unknown_data_class, quality_levels_count)
{
	constructor_impl						();
}

void   unmanaged_resource::constructor_impl ()
{
	m_next_delay_delete					=	NULL;
	m_deleter							=	NULL;
	m_fat_it							=	vfs::vfs_iterator::end();
	m_inlined_in_fat					=	false;
	m_next_in_global_list				=	NULL;
	m_prev_in_global_list				=	NULL;
	m_next_in_global_delay_delete_list	=	NULL;
	m_prev_in_global_delay_delete_list	=	NULL;
}

unmanaged_resource::~unmanaged_resource ()
{
	unlink_from_children					();
	if ( m_deleter )
	{
		R_ASSERT							(creation_source() != creation_source_unset);
		LOGI_INFO							("resources", "deleted %s", resources::log_string(this).c_str());
		g_resources_manager->debug()->unregister_unmanaged_resource	(this);
	}
}

void   unmanaged_resource::late_set_fat_it (vfs::vfs_iterator in_it)
{
	R_ASSERT								(in_it == m_fat_it || m_fat_it.is_end() || in_it.is_end());
	m_fat_it							=	in_it;	
}

void   unmanaged_resource::set_deleter_object (cook_base * const cook, u32 deallocation_thread_id)
{
	bool old_cook_was_present			=	(m_deleter != NULL);
	if ( m_deleter )
		m_deleter->change_cook_users_count	(-1);

	m_deleter							=	cook;
	m_class_id							=	cook ? cook->get_class_id() : raw_data_class;
	m_deallocation_thread_id			=	deallocation_thread_id;

	if ( cook )
	{
		if ( !old_cook_was_present )
			g_resources_manager->debug()->register_unmanaged_resource	(this);

		m_deleter->change_cook_users_count	(+1);
	}
	else if ( old_cook_was_present )
		g_resources_manager->debug()->unregister_unmanaged_resource		(this);
}

log_string_type   unmanaged_resource::log_string () const
{ 
#ifndef MASTER_GOLD
	pcstr request_path				=		m_request_path.c_str();
#else
	pcstr request_path				=		"<was not saved>";
#endif

	pvoid const this_ptr			=		dynamic_cast<pvoid>((unmanaged_resource*)this);

	u32 const size					=		get_size();
	log_string_type							out_result;
	if ( creation_source() == creation_source_physical_path || 
		 creation_source() == creation_source_user_data )
	{
		out_result.assignf					("unmanaged resource '%s', size = %d [ruid %d] [rptr 0x%x]", request_path, size, uid(), this_ptr);
		return								out_result;
	}
	
	if ( creation_source() == creation_source_created_by_user )
	{
		out_result.assignf					("%s: '%s', size = %d [ruid %d] [rptr 0x%x]", 
											 (m_class_id == vfs_sub_fat_class) ? "sub-fat" : "unmanaged user-resource",
											 request_path, size, uid(), this_ptr);

		return								out_result;
	}

	if ( creation_source() == creation_source_deallocate_buffer_helper )
	{
		out_result.assignf					("unmanaged resource buffer '%s', size = %d [ruid %d] [rptr 0x%x]", request_path, size, uid(), this_ptr);
		return								out_result;
	}

	if ( m_fat_it.is_end() )
	{
		out_result.assignf					("unmanaged resource '%s', size = %d [ruid %d] [rptr 0x%x]", request_path, size, uid(), this_ptr);

		return								out_result;
	}

	R_ASSERT								(creation_source() == creation_source_fat ||
											 creation_source() == creation_source_translate_query);
	out_result.assignf						("unmanaged resource '%s', size = %d [ruid %d] [rptr 0x%x]", m_fat_it.get_virtual_path().c_str(), size, uid(), this_ptr);

	return									out_result;
}

void   unmanaged_resource::set_creation_source (creation_source_enum creation_source, pcstr request_path, memory_usage_type const & memory_usage)
{ 
	R_ASSERT								(m_creation_source == creation_source_unset);
	m_creation_source					=	creation_source;
	set_memory_usage						(memory_usage);
	XRAY_UNREFERENCED_PARAMETER				(request_path);
#ifndef MASTER_GOLD
	m_request_path						=	request_path;
#endif
}

void   unmanaged_resource::set_as_inlined_in_fat ()
{
	m_inlined_in_fat					=	true;
	threading::interlocked_increment		(m_reference_count);
}

void   unmanaged_resource::unset_as_inlined_in_fat ()
{
	threading::interlocked_decrement		(m_reference_count);
	m_inlined_in_fat					=	false;
}

void   unmanaged_resource::set_sub_fat_resource (vfs_sub_fat_resource * sub_fat)
{	
	m_sub_fat.initialize_and_set_parent		(this, sub_fat);
}

unmanaged_resource_buffer::unmanaged_resource_buffer (resource_base::creation_source_enum creation_source, pcstr request_name, memory_usage_type memory_usage, class_id_enum class_id, query_result * destruction_observer, bool is_delay_delete, bool is_delay_deallocate)
:	m_creation_source(creation_source), m_memory_usage(memory_usage), m_class_id(class_id), m_next_to_deallocate(NULL), 
	m_is_delay_deallocate(is_delay_deallocate), m_is_delay_delete(is_delay_delete),
	m_next_in_global_delay_delete_list(NULL), m_prev_in_global_delay_delete_list(NULL),
	m_destruction_observer(destruction_observer)
{
#ifndef MASTER_GOLD
	m_request_name						=	request_name;
#else // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETER				( request_name );
#endif // #ifndef MASTER_GOLD
}

} // namespace resources
} // namespace xray
