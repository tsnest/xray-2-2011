////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include "test.h"
#include <xray/vfs/watcher.h>
#include "mount_archive.h"
#include "find.h"
#include "branch_locks.h"

namespace xray {
namespace vfs {

using namespace fs_new;

void   log_vfs_nodes					(base_node<> * node, u32 level = 0, pcstr original_name = NULL)
{
	if ( !node )
		return;

	if ( node->is_link() )
	{
		base_node<> * const referenced_node	=	find_referenced_link_node(node);
		log_vfs_nodes						(referenced_node, level, node->get_name());
		return;
	}

	fixed_string512	string;
	string.append_repeat					("  ", level);

	string								+=	original_name ? original_name : node->get_name();

	fixed_string512							type;

	if ( node->has_flags(vfs_node_is_mount_helper) )
	{
		type							=	"[helper]";
	}
	else 
	{
		mount_root_node_base<> * mount_root	=	node->is_mount_root() ? 
															cast_mount_root_node_base(node) : node->get_mount_root();

		pcstr const file_name			=	file_name_from_path(native_path_string(mount_root->physical_path));
		
		type							=	file_name;
// 		for ( u32 i=0; i<strings::length(file_name); ++i )
// 			if ( isdigit(file_name[i]) )
// 			{
// 				type.assignf				("%c", file_name[i]);
// 				break;
// 			}

		if ( node->is_mount_root() )
			type						+=	" [mount root]";
	}

	if ( node->has_flags(vfs_node_is_sub_fat) )
		type							=	" [sub-fat]";

	if ( node->is_erased() )
		type							+=	" [erased]";

	if ( original_name )
	{
		virtual_path_string					path;
		node->get_full_path					(& path);
		type.appendf						(" [link to: '%s']", path.c_str());
	}

	base_folder_node<> * folder			=	cast_folder(node);

	fixed_string512	folder_addr;
	if ( folder )
		folder_addr.assignf					(" folder[0x%08x]", folder);

	LOG_FORCED								(logging::trace, 
											 "%s	%s (%s) [0x%08x]%s", string.c_str(), type.c_str(), 
											 node->is_folder() ? "fold" : "file", node, folder_addr.c_str());

	if ( !folder )
		return;

	for ( base_node<> *	child	=	folder->get_first_child();
						child;
						child	=	child->get_next() )
	{
		log_vfs_nodes						(child, level + 1);
	}
}

void   log_vfs_root						(virtual_file_system & vfs, bool assert_empty)
{
	base_node<> * node					=	vfs.hashset.find_no_lock("", vfs_hashset::check_locks_false);
	if ( assert_empty )
		R_ASSERT							(!node);
 	if ( node )
  		log_vfs_nodes						(& * node);

	LOG_TRACE								("-------------------------------------"); 
}

} // namespace vfs
} // namespace xray