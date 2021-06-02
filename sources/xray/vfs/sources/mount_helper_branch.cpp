////////////////////////////////////////////////////////////////////////////
//	Created		: 08.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mounter.h"
#include "branch_locks.h"
#include "mount_utils.h"
#include "mount_transfer.h"
#include <xray/fs/path_string_utils.h>
#include <xray/vfs/virtual_file_system.h>
#include "overlapped_node_iterator.h"

namespace xray {
namespace vfs {

using namespace fs_new;

void   transfer_children_to_empty_folder (base_folder_node<> * dest_folder, base_folder_node<> * source_folder)
{
	for ( base_node<> * child			=	source_folder->get_first_child();
						child;
						child			=	child->get_next()	)
	{
		child->set_parent					(dest_folder);
	}

	dest_folder->set_first_child			(source_folder->get_first_child());
	source_folder->set_first_child			(NULL);
}

bool   is_mounted_automatic_archive			(base_node<> * node)
{
	physical_file_node<> * physical_file	=	node_cast<physical_file_node>(node);
	if ( !physical_file )
		return								false;

	if ( !physical_file->checked_is_archive() )
		return								false;

	return									physical_file->is_archive_file();
}

// during mount folders could still contain overlapped children marked with 'vfs_node_is_marked_to_unlink_from_parent' flag
// this functions removes them from children list and unsets 'vfs_node_is_marked_to_unlink_from_parent' flag
void   mounter::remove_marked_to_unlink_from_parent	(base_folder_node<> * parent)
{
	typedef	intrusive_list< base_node<>, base_node<>::node_pointer, & base_node<>::m_next, threading::single_threading_policy >
		node_list;

	base_node<> * const overlapped		=	node_cast<base_node>(parent)->get_next_overlapped();
	base_folder_node<> * const old_parent	=	(overlapped && !overlapped->is_link()) ? 
													node_cast<base_folder_node>(overlapped) : NULL;

	node_list	new_children;
	for ( base_node<> * child			=	parent->get_first_child();
						child;	)
	{
		base_node<> * next_child		=	child->get_next();
		R_ASSERT							(child->get_parent() == parent);

		if ( !child->has_flags(vfs_node_is_marked_to_unlink_from_parent) )
		{
			new_children.push_back			(child);
		}
		else
		{
			child->unset_flags				(vfs_node_is_marked_to_unlink_from_parent);
		
			R_ASSERT						(old_parent);
			R_ASSERT						(old_parent != parent);
			R_ASSERT						(!old_parent->find_child(child->get_name(), NULL));
			old_parent->prepend_child		(child);
		}

		child							=	next_child;
	}

	parent->set_first_child					(new_children.front());
}

// this functions overlaps 'previous_root_node' with new 'new_root_node' node
// and transfers its children to 'new_root_node'
void   mounter::merge_root_node_with_tree	(base_node<> * new_root_node, base_node<> * top_root_node, bool * added_ontop)
{
	R_ASSERT								(new_root_node != top_root_node);

	base_node<> * overlapper			=	NULL;
	base_node<> * overlapped			=	top_root_node;
	u32 const mount_id					=	mount_id_of_node(new_root_node);
	while ( overlapped && mount_id_of_node(overlapped) > mount_id )
	{
		overlapper						=	overlapped;
		overlapped						=	overlapped->get_next_overlapped();
	}
	
	if ( new_root_node->is_folder() )
		R_ASSERT							(!node_cast<base_folder_node>(new_root_node)->get_first_child());

	if ( overlapped && overlapped->is_folder() && new_root_node->is_folder() )
		transfer_children_to_empty_folder	(node_cast<base_folder_node>(new_root_node), node_cast<base_folder_node>(overlapped));

	if ( overlapper )
		overlapper->set_next_overlapped		(new_root_node);
		
	new_root_node->set_next_overlapped		(overlapped);

	if ( m_file_system.on_node_hides && overlapped )
	{
		vfs_iterator	iterator			(overlapped, NULL, & m_file_system.hashset, vfs_iterator::type_non_recursive);
		m_file_system.on_node_hides			(iterator);
	}

	ASSERT									(added_ontop);
	* added_ontop						=	!overlapper;
}

base_folder_node<> *   mounter::find_parent_to_link	(base_node<> * *				in_out_overlapper, 
													 base_node<> *					candidate_for_link, 
													 base_folder_node<> *			parent, 
													 virtual_path_string const &	path)
{
	if ( candidate_for_link && node_cast<base_folder_node>(candidate_for_link) == parent )
		return								parent;

	virtual_path_string						parent_path;
	get_path_without_last_item				(& parent_path, path.c_str());
	vfs_hashset::begin_end_pair begin_end	=	m_file_system.hashset.equal_range(parent_path.c_str(), lock_type_write);
	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second;

	bool found_candidate				=	false;
	for ( ; it != it_end; ++it )
		if ( !candidate_for_link || it.c_ptr() == candidate_for_link )
		{
			found_candidate				=	true;
			break;
		}

	R_ASSERT								(found_candidate);

	if ( * in_out_overlapper )
	{
		found_candidate					=	false;
		u32 const overlapper_mount_id	=	mount_id_of_node(* in_out_overlapper);
		for ( ; it != it_end; ++it )
		{
			base_node<> * const it_node	=	it.c_ptr();
			if ( mount_id_of_node(it_node) == overlapper_mount_id )
			{
				found_candidate			=	true;
				++it;
				R_ASSERT					(it != it_end);
				break;
			}
		}
		R_ASSERT							(found_candidate);
	}


	bool reached_file					=	false;
	candidate_for_link					=	it.c_ptr();
	base_node<> * prev_node				=	NULL;
	for ( ; it != it_end; prev_node = it.c_ptr(), ++it )
	{
		base_node<> * const it_node		=	it.c_ptr();

		if ( prev_node && !prev_node->get_next_overlapped() )
		{
			* in_out_overlapper			=	NULL;
			candidate_for_link			=	it_node;
		}

		if ( reached_file )
		{
			candidate_for_link			=	it_node;
			reached_file				=	false;
		}

		if ( it_node == node_cast<base_node>(parent) )
			break;

		if ( it_node->is_file() )
		{
			reached_file				=	true;
			* in_out_overlapper			=	NULL;
			continue;
		}
	}

	base_folder_node<> * const out_result	=	node_cast<base_folder_node>(candidate_for_link);
	R_ASSERT								(out_result);
	return									out_result;
}

bool   folders_connected_by_overlap		(base_node<> * overlapper, base_node<> * overlapped)
{
	base_node<> * it_node				=	NULL;
	for (	it_node						=	overlapper;
			it_node && it_node->is_folder() && it_node != overlapped;
			it_node						=	it_node->get_next_overlapped() )
	{;}

	return									it_node == overlapped;
}

void   mounter::find_overlapped_and_parent_to_link (base_folder_node<> * *		out_parent_to_link,
													base_node<> * *				out_overlapper, 
													base_node<> * *				out_overlapped, 													
													virtual_path_string const &	path, 
													u32 						hash,
													base_node<> *				node, 
													base_folder_node<> *		parent)
{
	vfs_hashset::begin_end_pair begin_end	=	m_file_system.hashset.equal_range(path.c_str(), hash, lock_type_write);

	u32 const node_mount_id				=	mount_id_of_node(node);

	base_folder_node<> * parent_to_link	=	NULL;
	base_node<> * candidate_for_link	=	NULL;
	base_node<> * overlapped			=	NULL;
	base_node<> * overlapper			=	NULL;

	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second;
	for ( ; it != it_end; ++it )
	{
		base_node<> * const it_node		=	it.c_ptr();
		base_node<> * parent_of_it_node	=	node_cast<base_node>(it_node->get_parent());
		if ( mount_id_of_node(it_node) <= node_mount_id )
		{
			if ( mount_id_of_node(parent_of_it_node) >= node_mount_id )
			{
				parent_to_link			=	it_node->get_parent();
				overlapped				=	it_node;
			}
			
			break;
		}

		candidate_for_link				=	parent_of_it_node;
		overlapper						=	it_node;
	}

	it.clear								();

	if ( !parent_to_link )
		parent_to_link					=	find_parent_to_link(& overlapper, candidate_for_link, parent, path);

	R_ASSERT								(parent_to_link);

	if ( overlapper )
	{
		base_node<> * const overlapper_parent	=	node_cast<base_node>(overlapper->get_parent());
		base_node<> * const parent_to_link_node	=	node_cast<base_node>(parent_to_link);
		if ( !folders_connected_by_overlap(overlapper_parent, parent_to_link_node) )
			overlapper					=	NULL; // not really an overlapper (parents are separated)
	}

	* out_overlapper					=	overlapper;
	* out_overlapped					=	overlapped;
	* out_parent_to_link				=	parent_to_link;
}

base_node<> *	mounter::find_topmost_overlapper	(virtual_path_string const &	path, 
													 u32 const						hash,
													 base_node<> * const			node)
{
	vfs_hashset::begin_end_pair begin_end	=	m_file_system.hashset.equal_range(path.c_str(), hash, lock_type_write);

	base_node<> * overlapper			=	NULL;

	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second;
	for ( ; it != it_end; ++it )
	{
		base_node<> * const it_node		=	it.c_ptr();

		if ( it_node->is_file() )
			overlapper					=	NULL;
		else if ( !overlapper )
			overlapper					=	it_node;

		if ( it_node == node )
			break;
	}

	return									overlapper;
}

// this functions merges 'node' into a tree represented by parent that was earlier merged 
// and is overlapping 'overlapped_by_parent' node
// 'node' may overlap some other node, mark it with 'vfs_node_is_marked_to_unlink_from_parent' flag
// and return in 'out_replaced_by_node' parameter
void   mounter::merge_node_with_tree	(virtual_path_string const &	path, 
										 u32 							hash,
										 base_node<> *					node, 
										 base_folder_node<> *			parent)
{
	R_ASSERT								(!node->get_first_child());
	u32 const node_mount_id				=	mount_id_of_node(node);

	base_folder_node<> * parent_to_link	=	NULL;
	base_node<> * overlapped			=	NULL;
	base_node<> * overlapper			=	NULL;

	find_overlapped_and_parent_to_link		(& parent_to_link, & overlapper, & overlapped, path, hash, node, parent);

	bool node_is_submount_root			=	false;
	if ( node->is_mount_root() )
	{
		archive_folder_mount_root_node<> *	mount_root	=	node_cast<archive_folder_mount_root_node>(node);
		node_is_submount_root			=	mount_root && mount_root->attach_node;
	}

	if ( overlapped )
	{
		base_folder_node<> * overlapped_folder	=	overlapped->get_parent();

		if ( overlapped_folder == parent && !node_is_submount_root )
		{
			R_ASSERT						(!overlapped->has_flags(vfs_node_is_marked_to_unlink_from_parent));
			overlapped->add_flags			(vfs_node_is_marked_to_unlink_from_parent);
		}
		else
		{
			overlapped_folder->unlink_child	(overlapped);

			if ( !node_is_submount_root )
			{
				base_node<> * overlapped_by_parent	=	node_cast<base_node>(parent)->get_next_overlapped();
				R_ASSERT					(overlapped_by_parent);
				base_folder_node<> * folder_overlapped_by_parent	=	node_cast<base_folder_node>(overlapped_by_parent);
				R_ASSERT					(folder_overlapped_by_parent);
				R_ASSERT					(!folder_overlapped_by_parent->find_child(overlapped->get_name(), NULL));
				folder_overlapped_by_parent->prepend_child	(overlapped);
			}
		}
	}

	parent_to_link->prepend_child			(node);

	if ( overlapper && overlapper->is_folder() && 
		 node->is_file() && 
		 overlapped && overlapped->is_folder() )
	{
		separate_folders_by_file_node		(m_file_system.hashset, path.c_str(), hash, overlapper, node_mount_id);//, overlapped);
	}

	if ( overlapper )
		overlapper->set_next_overlapped		(node);

	if ( overlapped && overlapped->is_folder() && node->is_folder() )
		transfer_children_to_empty_folder	(node_cast<base_folder_node>(node), node_cast<base_folder_node>(overlapped));

	if ( m_file_system.on_node_hides && overlapped )
	{
		vfs_iterator	iterator			(overlapped, NULL, & m_file_system.hashset, vfs_iterator::type_non_recursive);
		m_file_system.on_node_hides			(iterator);
	}

	if ( node_is_submount_root )
	{
		R_ASSERT							(overlapped);
		R_ASSERT							(overlapped->is_sub_fat() || overlapped->is_file());
		// case when we're expanding sub-fat or automatic archive
		// we replace file node with mount_root
		base_node<> * next_overlapped	=	overlapped->get_next_overlapped();
		node->set_next_overlapped			(next_overlapped);

		if ( !overlapper && m_args.root_write_lock == overlapped )
		{
			base_folder_node<> * node_folder	=	node_cast<base_folder_node>(node);
			R_ASSERT						(node_folder);
			node_folder->lock				(lock_type_write, lock_operation_lock);
			R_ASSERT						(node->m_parent->has_lock(lock_type_soft_write));
		}

		m_file_system.hashset.replace		(hash, node, overlapped, mount_id_of_node(node));

		base_node<> * topmost_overlapper	=	find_topmost_overlapper(path, hash, node);
		R_ASSERT							(topmost_overlapper);
		if ( next_overlapped && next_overlapped->is_folder() && topmost_overlapper )
			transfer_children				(m_file_system.hashset, path, hash, topmost_overlapper, next_overlapped);
	}
	else
	{
 		node->set_next_overlapped			(overlapped);
		m_file_system.hashset.insert		(hash, node, mount_id_of_node(node));
	}
}

void   mounter::add_mount_helper_node	(mount_helper_node<> *			in_out_helper_node,
										 virtual_path_string &			path, 
										 u32 							path_hash,
										 base_node<> * &				in_out_current_helper,
										 base_node<> * &				in_out_branch_lock)
{
	pcstr const name					=	fs_new::file_name_from_path(path);
	mount_helper_node<>::create_inplace		(in_out_helper_node, m_args.allocator, name, strings::length(name), m_mount_id);
	add_mount_helper_node_impl				(node_cast<base_node>(in_out_helper_node), path, path_hash, in_out_current_helper, in_out_branch_lock);
}

void   mounter::merge_root_node			(u32 path_hash, base_node<> * new_root_node, base_node<> * & root_node_lock)
{
	bool	added_ontop					=	false;
	merge_root_node_with_tree				(new_root_node, root_node_lock, & added_ontop);

	if ( added_ontop )
		lock_node							(new_root_node, lock_type_write, lock_operation_lock);

	m_file_system.hashset.insert			(path_hash, new_root_node, mount_id_of_node(new_root_node));

	if ( added_ontop )
	{
		if ( root_node_lock )
			unlock_node						(root_node_lock, lock_type_write);
		root_node_lock					=	new_root_node;
	}
}

void   mounter::add_mount_helper_node_impl	(base_node<> *				node_to_add,
											 virtual_path_string &		path, 
											 u32 						path_hash,
											 base_node<> * &			in_out_current_helper,
											 base_node<> * &			in_out_branch_lock)
{
	if ( in_out_current_helper && path.length() != 0 )
	{
		R_ASSERT							(in_out_current_helper->is_mount_helper());
		node_to_add->set_mount_helper_parent	(node_cast<mount_helper_node>(in_out_current_helper));
	}

	bool adding_at_root					=	(path.length() == 0);

	if ( adding_at_root )
	{
		R_ASSERT							(!in_out_current_helper);

		merge_root_node						(path_hash, node_to_add, in_out_branch_lock);
	}
	else
	{
		R_ASSERT							(in_out_current_helper);

		merge_node_with_tree				(path, path_hash, node_to_add, node_cast<base_folder_node>(in_out_current_helper));

		remove_marked_to_unlink_from_parent	(node_cast<base_folder_node>(in_out_current_helper));

		base_node<> * new_branch_lock	=	NULL;
		m_file_system.hashset.find_no_branch_lock	(new_branch_lock, path.c_str(), path_hash, lock_type_write);
		
		upgrade_node						(in_out_branch_lock, lock_type_write, lock_type_soft_write);

		in_out_branch_lock				=	new_branch_lock;
	}

	in_out_current_helper				=	node_to_add;
}

mount_root_node_base<> *   mounter::create_mount_root (bool mounting_folder, virtual_path_string const & path)
{
	pcstr root_name						=	file_name_from_path(path);

	mount_root_node_base<> * new_mount_root	=	NULL;
	if ( m_args.type == mount_type_archive )
	{
		new_mount_root					=	NULL;
		NODEFAULT							(return NULL);
	}
	else if ( m_args.type == mount_type_physical_path && mounting_folder )
	{
		physical_folder_mount_root_node<> * const root_node	=	
			mount_root_node_functions::create<physical_folder_mount_root_node, platform_pointer_default>
											(root_name, strings::length(root_name), & m_file_system, m_args);
		new_mount_root					=	root_node;
	}
	else if ( m_args.type == mount_type_physical_path && !mounting_folder )
	{
		physical_file_mount_root_node<> * const root_node	=	
			mount_root_node_functions::create<physical_file_mount_root_node, platform_pointer_default>
											(root_name, strings::length(root_name), & m_file_system, m_args);
		new_mount_root					=	root_node;
	}
	else
		NODEFAULT							();

	R_ASSERT								(m_mount_id != 0);
	new_mount_root->mount_id			=	m_mount_id;
	new_mount_root->mount_operation_id	=	next_mount_operation_id();

	return									new_mount_root;
}

void   mounter::add_mount_root			(mount_root_node_base<> *	new_mount_root,
										 virtual_path_string &		path, 
										 u32 *						out_hash,
										 base_node<> * &			in_out_branch,
										 base_node<> * &			in_out_lock)
{
	* out_hash							=	path_crc32(path);
	add_mount_helper_node_impl				(node_cast<base_node>(new_mount_root), path, * out_hash, in_out_branch, in_out_lock);
}

bool   mounter::allocate_mount_branch	(buffer_vector< mount_helper_node<> * > * out_helper_nodes)
{
	bool out_of_memory					=	false;

	for ( path_part_iterator	it		=	m_args.virtual_path.begin_part(include_empty_string_in_iteration_true), 
								it_end	=	m_args.virtual_path.end_part();
								it		!=	it_end; )
	{
		virtual_path_string					path_part;
		it.assign_to_string					(path_part);

		++it;

		if ( it == it_end )
			break;

		u32 const helper_node_size		=	sizeof(mount_helper_node<>) + path_part.length() + 1;
		mount_helper_node<> * const new_node	=	(mount_helper_node<> *)XRAY_MALLOC_IMPL(m_args.allocator, helper_node_size, "mount_helper_node");
		if ( !new_node )
		{
			out_of_memory				=	true;
			break;
		}

		out_helper_nodes->push_back		(new_node);
	}

	if ( !out_of_memory )
		return								true;

	free_mount_branch						(* out_helper_nodes);
	return									false;
}

void   mounter::free_mount_branch		(buffer_vector< mount_helper_node<> * > & helper_nodes)
{
	for ( u32 i=0; i<helper_nodes.size(); ++i )
		if ( helper_nodes[i] )
			XRAY_FREE_IMPL					(m_args.allocator, helper_nodes[i]);
}

void   mounter::add_mount_branch		(buffer_vector< mount_helper_node<> * > &	helper_nodes,
										 base_node<> * &							out_branch,
										 base_node<> * &							in_out_lock,
										 u32 *										out_node_hash)
{
	R_ASSERT								(!in_out_lock || !in_out_lock->get_parent());
	virtual_path_string						partial_path;
	u32 partial_path_hash				=	path_crc32(virtual_path_string(""));
	
	int index							=	0;
	for ( path_part_iterator	it		=	m_args.virtual_path.begin_part(include_empty_string_in_iteration_true), 
								it_end	=	m_args.virtual_path.end_part();
								it		!=	it_end; )
	{
		virtual_path_string					path_part;
		it.assign_to_string					(path_part);
		
		partial_path.append_path			(path_part);
		partial_path_hash				=	path_crc32(path_part, partial_path_hash);

		++it;

		if ( it != it_end )
			add_mount_helper_node			(helper_nodes[index], partial_path, partial_path_hash, out_branch, in_out_lock);
		++index;
	}

	if ( out_node_hash )
		* out_node_hash					=	partial_path_hash;
}

} // namespace vfs
} // namespace xray