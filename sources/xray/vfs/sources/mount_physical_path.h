////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MOUNT_PHYSICAL_PATH_H_INCLUDED
#define MOUNT_PHYSICAL_PATH_H_INCLUDED

#include "mounter.h"

namespace xray {
namespace vfs {

class	virtual_file_system;
class	query_mount_arguments;

class physical_path_mounter : public mounter
{
public:	
			physical_path_mounter		(query_mount_arguments & args, virtual_file_system & vfs_data);

private:
	void	mount_impl					();
	void	mount_root					();
	void	mount_lazy					();
	void	mount_hot					();
	void		mount_hot_branch		();
	base_node<> *   find_node_on_virtual_path	(base_node<> * * out_overlapper);
	void		hot_unmount				();

	void	mount_physical_folder		(fs_new::virtual_path_string &		folder_path,
										 physical_folder_node<> *			folder, 
										 fs_new::native_path_string &		absolute_path,
										 u32 const							folder_hash);

	void	flatten_helper_nodes_and_merge	(node_list &					new_nodes,
											 fs_new::virtual_path_string &	folder_path,
											 physical_folder_node<> *		folder, 
											 u32							folder_hash,
											 u32							folder_size,
											 fs_new::native_path_string &	absolute_path);

	void   free_node_list				(node_list & new_nodes);

private:
	base_node<> *	add_physical_node	(fs_new::virtual_path_string const &	name, 
										 fs_new::virtual_path_string const &	virtual_path,
										 u32									virtual_path_hash, 
										 fs_new::native_path_string const &		physical_path,
										 base_node<> *							parent);

private:
	fs_new::synchronous_device_interface *	m_device;

}; // struct physical_path_mounter

vfs_mount_ptr   mount_physical_path		(query_mount_arguments & args, virtual_file_system & vfs_data);

} // namespace vfs
} // namespace xray

#endif // #ifndef MOUNT_PHYSICAL_PATH_H_INCLUDED