////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_H_INCLUDED
#define VFS_MOUNT_H_INCLUDED

#include "nodes.h"
#include <xray/vfs/mount_args.h>
#include "mount_referers.h"
#include <xray/vfs/mounter_base.h>

namespace xray {
namespace vfs {

class mounter : public mounter_base, public core::noncopyable
{
public:
	mounter								(query_mount_arguments & args, virtual_file_system & file_system);
	virtual ~mounter					();

	mount_result	get_result			() const { return mount_result(m_mount_ptr, m_result); }
	query_mount_arguments const &		args () const { return m_args; }
	void	add_referer					(mount_referer * referer) { m_referers.push_back(referer); }
	bool	try_mount_from_history	();
protected:
	enum create_mount_root_bool			{ create_mount_root_false, create_mount_root_true };
	typedef	intrusive_list< base_node<>, base_node<>::node_pointer, & base_node<>::m_next, threading::single_threading_policy >
		node_list;

	void  add_mount_branch				(buffer_vector< mount_helper_node<> * > & helper_nodes,
										 base_node<> * &						out_branch,
										 base_node<> * &						in_out_lock,
										 u32 *									out_node_hash);

	bool   allocate_mount_branch		(buffer_vector< mount_helper_node<> * > * out_helper_nodes);
	void   free_mount_branch			(buffer_vector< mount_helper_node<> * > & helper_nodes);

	void   add_mount_root				(mount_root_node_base<> *				new_mount_root,
										 fs_new::virtual_path_string &			path, 
										 u32 *									out_hash,
										 base_node<> * &						in_out_branch,
										 base_node<> * &						in_out_lock);

	void  add_mount_helper_node			(mount_helper_node<> *					in_out_helper_node,
										 fs_new::virtual_path_string &			path, 
										 u32 									path_hash,
										 base_node<> * &						in_out_current_helper,
										 base_node<> * &						in_out_branch_lock);

	void   merge_node_with_tree			(fs_new::virtual_path_string const &	node_path, 
										 u32 									node_hash,
										 base_node<> *							node, 
										 base_folder_node<> * 					parent);
	void   find_overlapped_and_parent_to_link	(base_folder_node<> * *					out_parent_to_link,
												 base_node<> * *						out_overlapper, 
												 base_node<> * *						out_overlapped, 													
												 fs_new::virtual_path_string const &	path, 
												 u32 									hash,
												 base_node<> *							node, 
												 base_folder_node<> *					parent);

	base_folder_node<> *   find_parent_to_link	(base_node<> * *						in_out_overlapper, 
												 base_node<> *							candidate_for_link, 
												 base_folder_node<> *					parent, 
												 fs_new::virtual_path_string const &	path);

	base_node<> *   find_topmost_overlapper		(fs_new::virtual_path_string const &	path, 
												 u32 									hash,
												 base_node<> *							node);

	void   add_mount_helper_node_impl	(base_node<> *							node_to_add,
										 fs_new::virtual_path_string &			path, 
										 u32 									path_hash,
										 base_node<> * &						in_out_current_helper,
										 base_node<> * &						in_out_branch_lock);

	void   finish_with_out_of_memory	();

	void   finish						(mount_result const & result, bool reused_from_history = false);
	
	void   destroy_this_if_needed		();

	void   merge_root_node_with_tree	(base_node<> * new_root_node, base_node<> * top_root_node, bool * added_ontop);
	static void   remove_marked_to_unlink_from_parent	(base_folder_node<> * parent);
	void   merge_root_node				(u32 path_hash, base_node<> * new_root_node, base_node<> * & root_node_lock);

	mount_root_node_base<> *   create_mount_root (bool mounting_folder, fs_new::virtual_path_string const & path);

protected:
	typedef intrusive_list<mount_referer_base, mount_referer *, & mount_referer_base::next, threading::mutex>	referers_list;
	referers_list						m_referers;
	vfs_mount_ptr						m_mount_ptr;
	result_enum							m_result;
	query_mount_arguments 				m_args;
	mount_root_node_base<> *			m_mount_root_base;
	virtual_file_system &				m_file_system;
	u32									m_mount_id;

}; // class vfs_mount

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_H_INCLUDED