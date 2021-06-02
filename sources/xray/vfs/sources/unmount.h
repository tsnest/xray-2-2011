////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_UNMOUNT_H_INCLUDED
#define VFS_UNMOUNT_H_INCLUDED

#include "nodes.h"
#include <xray/vfs/mount_args.h>
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

class unmounter : core::noncopyable
{
public:
	unmounter									(query_mount_arguments &		args, 
												 virtual_file_system &			file_system);

private:
	void	unmount								();
	void	hot_unmount							();
	void		hot_unmount_node				(base_node<> * node, u32 hash);
	// unmount nodes that points to mount root
	mount_root_node_base<> *	find_mount_root (u32 virtual_path_hash);

	// ----unmount mount's nodes----
	template <class Predicate> inline
	void	recursive_unmount_node				(fs_new::virtual_path_string &	path, 
												 u32 const						hash,
												 Predicate &					predicate,
												 base_node<> * &				node_to_unmount,
												 base_node<> * &				overlap_of_node_to_unmount);
	
	template <class Predicate> inline
	void		recursive_unmount_folder_range	(fs_new::virtual_path_string &	path, 
												 u32 const						hash,
												 Predicate &					predicate,
												 base_node<> *					first_to_unmount,
												 base_node<> *					last_to_unmount);

	template <class Predicate> inline
	void		recursive_unmount_folder		(fs_new::virtual_path_string &	path, 
										    	 u32 const						hash,
												 Predicate const &				predicate,
										    	 base_folder_node<> *			parent_to_unmount);
	template <class Predicate> inline
	void		recursive_traverse_folder		(fs_new::virtual_path_string &	path, 
										    	 u32 const						hash,
												 Predicate const &				predicate,
										    	 base_folder_node<> *			parent_to_traverse);
	template <class Predicate> inline
	void		find_range_to_unmount			(fs_new::virtual_path_string &	path, 
												 u32 const						hash,
												 Predicate const &				predicate,
												 base_node<> * &				first_to_unmount,		
												 base_node<> * &				last_to_unmount,
												 base_node<> * &				next_to_last);
	
	void   unmount_helper_branch				(mount_helper_node<> *			parent_to_unmount, 
												 base_node<> *					child_to_unmount,
												 base_node<> *					overlap_of_unmount,
												 u32							child_to_unmount_hash);
	void		unmount_helper					(fs_new::virtual_path_string &	path, 
												 u32 const						hash,
												 base_node<> *					parent_to_unmount,
												 base_node<> *					node_to_unmount,
												 base_node<> * &				out_overlap_of_node_to_unmount);

	void		unlink_from_attach_node			(base_node<> * mount_root, u32 hash, base_node<> * overlap_of_mount_root, base_node<> * attach_node);

private:
	query_mount_arguments &				m_args;
	vfs_hashset &						m_hashset;
	virtual_file_system &				m_file_system;
	mount_root_node_base<> *			m_root_node_to_unmount;

	friend struct						unmount_mount_root_nodes;
}; // class unmounter


inline
bool   unmount							(query_mount_arguments & args, virtual_file_system & file_system)
{
	class unmounter			helper			(args, file_system);
	return									true;
}

} // namespace vfs
} // namespace xray

#include "unmount_inline.h"


#endif // #ifndef VFS_UNMOUNT_H_INCLUDED