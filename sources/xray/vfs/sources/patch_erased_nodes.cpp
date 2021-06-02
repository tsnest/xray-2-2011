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

void	mount_nodes_for_patch::insert_erased	(virtual_path_string const &	path,
												 vfs_iterator &					patch_iterator, 
												 vfs_iterator &					parent_patch_iterator, 
												 vfs_iterator &					from_iterator)
{
	vfs_iterator const to_iterator		=	path.length() ? m_to_root.find_child(path) : m_to_root;
	if ( from_iterator && to_iterator )
	{
		if ( from_iterator.is_file() || to_iterator.is_file() )
			return;

		for ( vfs_iterator	from_child	=	from_iterator.children_begin(),
							end_child	=	from_iterator.children_end();
							from_child	!=	end_child;
							++from_child )
		{
			pcstr const child_name		=	from_child.get_name();
			virtual_path_string	child_path	=	path;
			child_path.append_path			(child_name);

			vfs_iterator patch_child	=	patch_iterator ?
												patch_iterator.find_child(child_name) : vfs_iterator::end();

			insert_erased					(child_path.c_str(), patch_child, patch_iterator, from_child);
		}

		return;
	}
	
	R_ASSERT								(from_iterator);
	R_ASSERT								(!to_iterator);
	R_ASSERT								(!patch_iterator);

	base_node<> * parent				=	parent_patch_iterator.get_node();

	if ( !parent )
		parent							=	create_branch_for_node(path);

	mount_root_node_base<> * const mount_root	=	parent->get_mount_root();

	erased_node<> * const erased		=	vfs::erased_node<>::create(m_args.allocator, mount_root, from_iterator.get_name());
	base_node<> * const erased_node		=	node_cast<base_node>(erased);

	base_folder_node<> * const parent_folder	=	node_cast<base_folder_node>(parent);
	parent_folder->prepend_child			(erased_node);

	u32 const hash						=	path_crc32(path);
	m_vfs->hashset.insert					(hash, erased_node, mount_root->mount_id);
}

} // namespace vfs
} // namespace xray