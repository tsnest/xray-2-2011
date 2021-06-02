////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MOUNT_ARCHIVE_H_INCLUDED
#define MOUNT_ARCHIVE_H_INCLUDED

#include <xray/fs/asynchronous_device_interface.h>
#include <xray/detail_noncopyable.h>
#include "mounter.h"
#include <xray/vfs/mount_ptr.h>
#include <xray/vfs/mount_args.h>
#include "mount_archive.h"

namespace xray {
namespace vfs {

class	virtual_file_system;
class	query_mount_arguments;

class archive_mounter : public mounter
{
public:

			archive_mounter				(query_mount_arguments & args, virtual_file_system & file_system);
private:

	void	mount_sub_fat				();
	bool	read_sub_fat				(fs_new::synchronous_device_interface &		device);
	void	on_read_sub_fat				(bool read_result);
	void	mount_archive				();
	void	mount_archive_impl			(fs_new::synchronous_device_interface &		device);
	void	mount_archive_to_parent		(fs_new::file_type *						fat_file, 
										 base_folder_node<> *						parent_of_mount_root, 
										 fs_new::synchronous_device_interface &		device);

	void	mount_fat					(archive_folder_mount_root_node<> * mount_root, base_folder_node<> * parent);
	void		recursive_fixup_node	(base_node<> * node, pstr buffer_origin);
	void		recursive_merge			(fs_new::virtual_path_string &		path, 
										 u32 								hash,
										 base_node<> *						node,
										 base_folder_node<> *				parent);

	mount_root_node_base<> *			m_mount_root_base;
	pstr								m_nodes_buffer;
	bool								m_reverse_byte_order;
	base_node<> *						m_branch_lock;

}; // class archive_mounter

vfs_mount_ptr   mount_archive			(query_mount_arguments & args, virtual_file_system & vfs_data);

} // namespace vfs
} // namespace xray

#endif // #ifndef MOUNT_ARCHIVE_H_INCLUDED