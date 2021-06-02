////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include "resources_manager.h"
#include <xray/managed_allocator.h>

namespace xray {
namespace resources {

void   query_result::init_replication	(vfs::vfs_iterator fat_it, managed_resource_ptr resource) 
{
	R_ASSERT								(resource);
	unset_flag								(flag_load | flag_save);
	set_flag								(flag_replication);
	m_fat_it							=	fat_it;
	m_raw_managed_resource				=	resource;
}

void   query_result::init_save			(save_generated_data *	save_data, 
										 query_result *			data_to_save_generator) 
{
	m_data_to_save_generator			=	data_to_save_generator;
	unset_flag								(flag_load | flag_replication);
	set_flag								(flag_save);
	strings::copy							(m_request_path, m_request_path_max_size, save_data->get_virtual_path());
	m_save_generated_data				=	save_data;
}

void   query_result::replicate ()
{
	R_ASSERT							( is_load_type() && is_successful() );

	managed_resource_ptr				replication_resource;

	if ( !cook_base::does_create_resource(m_class_id) )
	{
		replication_resource		=	m_raw_managed_resource;
	}
	else
	{
		replication_resource		=	g_resources_manager->allocate_managed_resource(m_raw_managed_resource->get_size(), raw_data_class);

		pinned_ptr_const<u8>	src_data		(m_raw_managed_resource);

		if ( replication_resource )
		{
			pinned_ptr_const<u8>		dest_data	(replication_resource);
			memory::copy				((pvoid)dest_data.c_ptr(), dest_data.size(), (pvoid)src_data.c_ptr(), src_data.size());
		}
		else
		{
			// no memory, synchronous write
#pragma message(XRAY_TODO("Lain 2 Lain: implement replication"))
			//fs::g_fat->replicate_file	(m_fat_it.get_fat_node(), src_data.c_ptr());

			LOGI_INFO("resources", "synchronously replicated due to low memory %s", 
											m_raw_managed_resource->log_string ().c_str());
			return;
		}
	}

	g_resources_manager->replicate_resource (m_fat_it, replication_resource, NULL);
}

} // namespace resources
} // namespace xray

