////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_managed_resource.h>
#include "resources_allocators.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

//----------------------------------------------------------
// managed_resource
//----------------------------------------------------------

managed_resource::managed_resource (u32	const size, class_id_enum const class_id) 
:	resource_base	(resource_flag_is_managed_resource, class_id), 
	managed_node_owner(& memory::g_resources_managed_allocator),
	m_next_delay_delete(NULL)
{
	m_memory_usage_self.type			=	& managed_memory;
	m_memory_usage_self.size			=	size;
}

managed_resource::~managed_resource ()
{
	unlink_from_children					();
	R_ASSERT								(creation_source() != creation_source_unset);
}

void   managed_resource::on_managed_node_resized (u32 const new_size)
{
	R_ASSERT								(new_size <= m_memory_usage_self.size);
	m_memory_usage_self.size			=	new_size;
}

void   managed_resource::late_set_fat_it (vfs::vfs_iterator new_it)
{
	if ( new_it == m_fat_it )
		return;
	R_ASSERT								(m_fat_it.is_end() || new_it.is_end());
	m_fat_it							=	(new_it);
}

log_string_type   managed_resource::log_string () const
{
#ifndef MASTER_GOLD
	pcstr request_path				=		m_request_path.c_str();
#else
	pcstr request_path				=		"<was not saved>";
#endif
	u32 const size					=		get_size();

	pvoid const this_ptr			=		dynamic_cast<pvoid>((managed_resource*)this);

	fixed_string512							out_result;
	if ( creation_source() == creation_source_physical_path ||
		 creation_source() == creation_source_user_data )
	{
		out_result.assignf					("managed resource : '%s', size = %d [ruid %d] [rptr 0x%x]", request_path, size, uid(), this_ptr);
		return								out_result;
	}

	fs_new::virtual_path_string	const full_path	=	get_virtual_path();

	if ( m_fat_it.is_end() )
	{
		out_result.assignf					("managed resource '%s', size = %d [ruid %d] [rptr 0x%x]", request_path, m_memory_usage_self.size, uid(), this_ptr);
		return								out_result;
	}

	R_ASSERT								(creation_source() == creation_source_fat ||
											 creation_source() == creation_source_translate_query);

	if ( !m_fat_it.is_end() && m_fat_it.is_compressed() && size == m_fat_it.get_raw_file_size() )
	{
		out_result.appendf					("managed compressed resource '%s', uncompressed = %d, compressed = %d [ruid %d] [rptr 0x%x]", 
											 full_path.c_str(),
											 m_fat_it.get_file_size(),
											 size,
											 uid(), 
											 this_ptr);
	}
	else
	{
		out_result.appendf					("managed resource '%s', size = %d, raw_file_size = %d [ruid %d] [rptr 0x%x]", 
											 full_path.c_str(),
											 size,
											 m_fat_it.is_end() ? 0 : m_fat_it.get_file_size(),
											 uid(),
											 this_ptr);
	}

	return									out_result;
}

void   managed_resource::set_creation_source (creation_source_enum creation_source, pcstr request_path)
{ 
	R_ASSERT								(m_creation_source == creation_source_unset); 
	m_creation_source					=	creation_source;
#ifndef MASTER_GOLD
	m_request_path						=	request_path;
#else // #ifndef MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETER				( request_path );
#endif // #ifndef MASTER_GOLD
}

void   managed_resource::set_sub_fat_resource (vfs_sub_fat_resource * sub_fat)
{	
	m_sub_fat.initialize_and_set_parent			(this, sub_fat);
}

} // namespace resources
} // namespace xray