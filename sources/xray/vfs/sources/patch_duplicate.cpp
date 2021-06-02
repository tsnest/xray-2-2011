////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "patch.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

using namespace fs_new;

base_node<> *   mount_nodes_for_patch::duplicate_to_patch	(base_node<> * const			original,
															 base_folder_node<> *			patch_parent, 
															 virtual_path_string const &	path)
{
	base_node<> * new_node				=	NULL;
	if ( original->is_folder() )
	{
		base_folder_node<> * const new_folder	=	base_folder_node<>::create(m_args.allocator, m_mount_root, 
																			   original->get_name());
		new_node						=	node_cast<base_node>(new_folder);
	}
	else
	{

		native_path_string const physical_path	=	get_node_physical_path(original);
		file_size_type const file_offs	=	get_file_offs(original);
		u32 const raw_file_size			=	get_raw_file_size(original);
		u32 const uncompressed_file_size	=	get_file_size(original);

		universal_file_node<> * const uni_node	=	universal_file_node<>::create
												(m_args.allocator, m_mount_root, original->get_name(), physical_path, 
												 raw_file_size, file_offs, original->is_compressed(), uncompressed_file_size);

		new_node						=	node_cast<base_node>(uni_node);
	}

	patch_parent->prepend_child				(new_node);
	u32 const hash						=	path_crc32(path);
	m_vfs->hashset.insert					(hash, new_node, m_mount_root->mount_id);

	return									new_node;
}

void   mount_nodes_for_patch::duplicate_children_to_patch	(base_folder_node<> *			patch_parent, 
															 virtual_path_string const &	path)
{
	vfs_iterator const to_iterator		=	m_to_root.find_child(path);
	R_ASSERT							(to_iterator);
	for ( vfs_iterator	to_child	=	to_iterator.children_begin(),
						end_child	=	to_iterator.children_end();
						to_child	!=	end_child;
						++to_child )
	{
		base_node<> * const to_child_node	=	to_child.data_node();

		virtual_path_string child_path	=	path;
		child_path.append_path				(to_child_node->get_name());

		base_node<> * const patch_child	=	duplicate_to_patch(to_child_node, patch_parent, child_path);

		if ( patch_child->is_folder() )
		{
			base_folder_node<> * child_folder	=	node_cast<base_folder_node>(patch_child);
			virtual_path_string	child_path	=	path;
			child_path.append_path			(patch_child->get_name());

			duplicate_children_to_patch		(child_folder, child_path);
		}
	}
}

base_node<> *    mount_nodes_for_patch::create_branch_for_node	(virtual_path_string const &	path)
{
	virtual_path_string						parent_path;
	get_path_without_last_item				(& parent_path, path.c_str());

	path_part_iterator	it				=	path_part_iterator(parent_path.c_str(), 
														   virtual_path_string::separator, 
														   include_empty_string_in_iteration_true);

	path_part_iterator	it_end			=	path_part_iterator::end();

	base_node<> * const root_node		=	m_vfs->hashset.find_no_lock("", vfs_hashset::check_locks_false);
	R_ASSERT								(root_node, "patch vfs should have a root node!");
	mount_root_node_base<> * const mount_root	=	root_node->get_mount_root();
	base_folder_node<> * parent			=	node_cast<base_folder_node>(root_node);
	for ( ; it != it_end; ++it )
	{
		virtual_path_string					name;
		it.assign_to_string					(name);

		virtual_path_string					partial_path;
		partial_path.assign					(parent_path.c_str(), it.cur_end());

		base_node<> * node				=	m_vfs->hashset.find_no_lock(partial_path.c_str(), vfs_hashset::check_locks_false);
		if ( !node )
		{
			base_folder_node<> * new_folder	=	base_folder_node<>::create(m_args.allocator, mount_root, name);
			node						=	node_cast<base_node>(new_folder);
			parent->prepend_child			(node);
			u32 const hash				=	path_crc32(partial_path);
			m_vfs->hashset.insert			(hash, node, mount_root->mount_id);

			vfs_iterator const from_it	=	m_from_root.find_child(partial_path);

			if ( from_it && from_it.get_node()->is_link() )
				duplicate_children_to_patch	(new_folder, partial_path);
		}

		parent							=	node_cast<base_folder_node>(node);
	}

	return									node_cast<base_node>(parent);
}

} // namespace vfs
} // namespace xray