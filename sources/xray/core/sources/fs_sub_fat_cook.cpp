////////////////////////////////////////////////////////////////////////////
//	Created		: 08.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include <xray/resources_resource_base.h>
#include <xray/fs_sub_fat_resource.h>

#include "fs_sub_fat_cook.h"
#include "fs_file_system.h"
#include "resources_helper.h"

namespace xray {
namespace fs {

using namespace		resources;

sub_fat_cook::sub_fat_cook () 
	: resources::inplace_unmanaged_cook(resources::sub_fat_class, 
										reuse_true, 
										use_resource_manager_thread_id, 
										use_resource_manager_thread_id,
										flag_destroy_in_any_thread | flag_no_deallocate)
{
}

mutable_buffer	sub_fat_cook::allocate_resource		(resources::query_result_for_cook &	in_query, 
													 u32								file_size, 
													 u32 &								out_offset_to_file, 
													 bool								file_exist) 
{
	R_ASSERT_U						(file_exist);
	u32 const resource_size		=	file_size + sizeof(sub_fat_resource);
	pvoid const buffer			=	UNMANAGED_ALLOC(char, resource_size);
	if ( !buffer )
	{
		in_query.set_out_of_memory	(resources::unmanaged_memory, resource_size);
		return						mutable_buffer::zero();
	}

	out_offset_to_file			=	sizeof(sub_fat_resource);

	return							mutable_buffer(buffer, resource_size);
}

void   sub_fat_cook::deallocate_resource (pvoid buffer)
{
	// is used to deallocate helper objects
	UNMANAGED_FREE					(buffer);
}

void   sub_fat_cook::create_resource (resources::query_result_for_cook &	in_out_query, 
									  mutable_buffer						in_out_unmanaged_resource_buffer)
{
	fat_header * const header		=	(fat_header *)((pbyte)in_out_unmanaged_resource_buffer.c_ptr() + sizeof(sub_fat_resource));
	u32 const raw_file_size			=	in_out_unmanaged_resource_buffer.size() - sizeof(sub_fat_resource);
	sub_fat_resource * const out_resource	=	new (in_out_unmanaged_resource_buffer.c_ptr()) 
												sub_fat_resource(* header, raw_file_size);

// 	path_string							logical_path_to_mount;
// 	directory_part_from_path			(& logical_path_to_mount, in_out_query.get_requested_path());
// 	out_resource->logical_path		=	logical_path_to_mount;

	out_resource->virtual_path		=	in_out_query.get_requested_path();

	resources::fs_iterator fat_it	=	static_cast_checked<query_result &>(in_out_query).get_fat_it();
	out_resource->sub_fat_node		=	fat_it.get_fat_node();
	R_ASSERT							(!fat_it.is_end());
	fat_it.get_disk_path				(out_resource->archive_physical_path);
	out_resource->fat_physical_path	=	out_resource->archive_physical_path;
	
	in_out_query.set_unmanaged_resource	(out_resource, 
										 resources::unmanaged_memory, 
										 in_out_unmanaged_resource_buffer.size());

	in_out_query.finish_query			(result_success);
}

void   sub_fat_cook::destroy_resource (resources::unmanaged_resource * resource)
{
	sub_fat_resource * const sub_fat	=	static_cast_checked<sub_fat_resource *>(resource);
	if ( !sub_fat->schedule_unmount_on_dying )
		return;

	query_unmount_db					(sub_fat->virtual_path.c_str(), 
										 sub_fat->archive_physical_path.c_str(), 
										 NULL, 
										 & memory::g_mt_allocator);
}

} // namespace fs
} // namespace xray
