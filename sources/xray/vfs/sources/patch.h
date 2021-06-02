////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_PATCH_H_INCLUDED
#define VFS_PATCH_H_INCLUDED

#include <xray/vfs/pack_archive.h>
#include <xray/vfs/patch_args.h>
#include <xray/vfs/locked_iterator.h>
#include "nodes.h"
#include <xray/compressor.h>

namespace xray {
namespace vfs {

struct	mount_nodes_for_patch : boost::noncopyable
{
public:
			mount_nodes_for_patch		(patch_args &							args,
										 compressor * const						compressor,
										 virtual_file_system * const			in_out_patch_vfs,
										 vfs_iterator &							from_iterator, 
										 vfs_iterator &							to_iterator);
private:
	void	insert_new_for_children		(fs_new::virtual_path_string const &	path, 
										 vfs_iterator &							from_iterator, 
										 vfs_iterator &							to_iterator);

	void	insert_new					(fs_new::virtual_path_string const &	path, 
										 vfs_iterator &							from_iterator, 
										 vfs_iterator &							to_iterator);

	void	insert_erased				(fs_new::virtual_path_string const &	path,
										 vfs_iterator &							patch_iterator, 
										 vfs_iterator &							parent_patch_iterator, 
										 vfs_iterator &							from_iterator);

	base_node<> *	duplicate_to_patch	(base_node<> *							original,
										 base_folder_node<> *					patch_parent, 
										 fs_new::virtual_path_string const &	path);
	void	duplicate_children_to_patch	(base_folder_node<> *					patch_parent, 
										 fs_new::virtual_path_string const &	path);

	base_node<> *   create_branch_for_node	(fs_new::virtual_path_string const &	path);

private:
	virtual_file_system * const			m_vfs;
	patch_args &						m_args;
	vfs_iterator						m_from_root;
	vfs_iterator						m_to_root;
	compressor *						m_compressor;
	mount_root_node_base<> *			m_mount_root;
};

void   unmount_nodes_for_patch			(virtual_file_system & patch_vfs);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_PATCH_H_INCLUDED