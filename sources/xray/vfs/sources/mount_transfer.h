////////////////////////////////////////////////////////////////////////////
//	Created		: 19.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_TRANSFER_H_INCLUDED
#define VFS_MOUNT_TRANSFER_H_INCLUDED

#include "mount_utils.h"

namespace xray {
namespace vfs {

class transfer_children : public core::noncopyable
{
public:
	transfer_children						(vfs_hashset &							hashset,
											 fs_new::virtual_path_string const &	path, 
											 u32									hash,
											 base_node<> *							dest_start, 
											 base_node<> *							source_folder);

private:
	void   find_first_and_last_overlapper	(base_node<> * *						out_first_overlapper,
											 base_node<> * *						out_last_overlapper,
											 fs_new::virtual_path_string const &	path, 
											 u32 const								hash,
											 base_node<> * const					child);


	void   transfer_child					(fs_new::virtual_path_string const &	path, 
											 u32 const								hash,
											 base_node<> * const					child);
private:
	vfs_hashset &						m_hashset;
	node_list							m_new_source_nodes;
	base_node<> *						m_dest_start;
	base_folder_node<> *				m_source_folder;
	u32									m_hash;
	fs_new::virtual_path_string			m_path;

}; // transfer_children


} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_TRANSFER_H_INCLUDED