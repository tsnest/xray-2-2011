////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/file_type_pointer.h>
#include "mount_archive.h"
#include "fat_header.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

using namespace fs_new;

void   archive_mounter::mount_sub_fat	()
{
	R_ASSERT								(m_args.submount_node);
	R_ASSERT								(m_args.parent_of_submount_node);
	R_ASSERT								(m_args.submount_node->is_archive() && m_args.submount_node->is_sub_fat());
	u32	const sub_fat_size				=	get_file_size(m_args.submount_node);
	R_ASSERT								(sub_fat_size);

	LOG_DEBUG								("mount started %ssubfat '%s' on '%s'", 
											 m_args.submount_node->is_external_sub_fat() ? "external " : "",
											 m_args.get_physical_path().c_str(),
 											 m_args.virtual_path.c_str());

	m_nodes_buffer						=	(pstr)XRAY_MALLOC_IMPL(m_args.allocator, sub_fat_size, "sub-fat nodes");
	if ( !m_nodes_buffer )
	{
		finish_with_out_of_memory			();
		return;
	}

	if ( m_args.asynchronous_device )
	{
		query_custom_operation_args	args	(boost::bind(& archive_mounter::read_sub_fat, this, _1),
 											 boost::bind(& archive_mounter::on_read_sub_fat, this, _1));
 		if ( !m_args.asynchronous_device->query_custom_operation(args, m_args.allocator) )
		{
			finish_with_out_of_memory		();
			return;
		}
	}
	else
	{
		R_ASSERT							(m_args.synchronous_device);
		bool const read_result			=	read_sub_fat(* m_args.synchronous_device);
		on_read_sub_fat						(read_result);
	}
}

bool   archive_mounter::read_sub_fat	(synchronous_device_interface & device)
{
	file_size_type	file_offs			=	get_file_offs(m_args.submount_node);

	if ( !m_args.submount_node->is_external_sub_fat() )
		R_ASSERT							(file_offs);

	native_path_string const fat_physical_path	=	m_args.get_physical_path();	

	file_type_pointer fat_file				(fat_physical_path.c_str(), device, file_mode::open_existing, file_access::read, assert_on_fail_false);
	if ( !fat_file )
 		return								false;

	device->seek							(fat_file, file_offs, seek_file_begin);

	u32 const sub_fat_size				=	get_file_size(m_args.submount_node);

	u32 read_bytes						=	(u32)device->read(fat_file, m_nodes_buffer, sub_fat_size);
	R_ASSERT_U								(read_bytes == sub_fat_size);

	return									true;
}

void   archive_mounter::on_read_sub_fat	(bool read_result)
{
	R_ASSERT_U								(read_result);

	archive_folder_mount_root_node<> *		mount_root	=	reinterpret_cast<archive_folder_mount_root_node<> *>
															((pstr)m_nodes_buffer + sizeof(fat_header));
	base_folder_node<> * root_parent	=	node_cast<base_folder_node>(m_args.parent_of_submount_node);

	mount_root->mount_size				=	get_file_size(m_args.submount_node);

	mount_fat								(mount_root, root_parent);

	add_to_mount_history					(m_mount_ptr.c_ptr(), m_file_system);

	LOG_INFO								("mounted %ssub_fat '%s' on '%s'", 
											 m_args.submount_node->is_external_sub_fat() ? "external " : "",
											 m_args.get_physical_path().c_str(),
 											 m_args.virtual_path.c_str());

	finish									(get_result());
	destroy_this_if_needed					();
}

} // namespace vfs
} // namespace xray