////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_UNMOUNT_INLINE_H_INCLUDED
#define VFS_UNMOUNT_INLINE_H_INCLUDED

#include <xray/fs/path_string_utils.h>
#include "mount_utils.h"
#include "free_node.h"
#include "mount_transfer.h"
#include "overlapped_node_iterator.h"

namespace xray {
namespace vfs {

struct is_part_of_mount
{
			is_part_of_mount			(mount_root_node_base<> * mount_root) : mount_root(mount_root) {}

	bool	operator ()					(base_node<> * node) const { return node->get_mount_root() == mount_root; }

	mount_root_node_base<> *			mount_root;
};

struct is_exact_node
{
			is_exact_node				(base_node<> * helper_node) : helper_node(helper_node) {}
	bool	operator ()					(base_node<> * node) const { return node == helper_node; }
	base_node<> *						helper_node;
};

template <class Predicate> inline
void   unmounter::recursive_unmount_folder_range (fs_new::virtual_path_string &	path, 
												  u32 const						hash,
												  Predicate &					predicate,
												  base_node<> *					first_to_unmount,
												  base_node<> *					last_to_unmount)
{
	overlapped_nodes_pair begin_end		=	m_hashset.equal_range(path.c_str(), hash, lock_type_write);
	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second;
	while ( it.c_ptr() != first_to_unmount )
		++it;

	base_node<> * current_to_unmount	=	first_to_unmount;
	do
	{
		if ( it != it_end )
			++it;
		base_node<> * next_node			=	it.c_ptr();
		R_ASSERT							(current_to_unmount->is_folder());

		if ( current_to_unmount == last_to_unmount )
			recursive_unmount_folder		(path, hash, predicate, node_cast<base_folder_node>(current_to_unmount));
		else
			recursive_traverse_folder		(path, hash, predicate, node_cast<base_folder_node>(current_to_unmount));
	
		if ( current_to_unmount == last_to_unmount )
			break;
		current_to_unmount				=	next_node;

	} while ( identity(true) );
}

template <class Predicate> inline
void   unmounter::find_range_to_unmount	(fs_new::virtual_path_string &	path, 
										 u32 const						hash,
										 Predicate const &				predicate,
										 base_node<> * &				first_to_unmount,		
										 base_node<> * &				last_to_unmount,
										 base_node<> * &				next_to_last)
{
	overlapped_nodes_pair begin_end		=	m_hashset.equal_range(path.c_str(), hash, lock_type_write);
	overlapped_node_iterator	it		=	begin_end.first;
	overlapped_node_iterator	it_end	=	begin_end.second;
	first_to_unmount					=	NULL;
	last_to_unmount						=	NULL;
	next_to_last						=	NULL;

	for ( ; it != it_end; ++it )
	{
		base_node<> *	node			=	& * it;

		if ( predicate(node) )
		{
			if ( !first_to_unmount )
				first_to_unmount		=	node;
			last_to_unmount				=	node;
			return;
		}

		next_to_last					=	node;

		if ( node->is_folder() && !first_to_unmount )
			first_to_unmount			=	node;
		else if ( node->is_file() )
			first_to_unmount			=	NULL;
	}
}

template <class Predicate> inline
void   unmounter::recursive_traverse_folder (fs_new::virtual_path_string &	path, 
										     u32 const						hash,
											 Predicate const &				predicate,
										     base_folder_node<> *			parent_to_traverse)
{
	u32 saved_path_length				=	path.length();
	node_list								new_children;
	for ( base_node<> * child			=	parent_to_traverse->get_first_child();
						child; )
	{
		base_node<> * const next_child	=	child->get_next();

		u32 child_hash					=	fs_new::path_crc32(fs_new::virtual_path_string(child->get_name()), hash);
		fs_new::virtual_path_string & child_path	=	path;
		child_path.append_path				(child->get_name());

		if ( predicate(child) )
		{
			base_node<> * child_to_unmount	=	NULL;
			base_node<> * overlap_of_child_to_unmount	=	NULL;
			recursive_unmount_node			(child_path, child_hash, predicate, child_to_unmount, overlap_of_child_to_unmount);
			R_ASSERT						(child_to_unmount == child);

			if ( overlap_of_child_to_unmount )
				overlap_of_child_to_unmount->set_next_overlapped(child_to_unmount->get_next_overlapped());

			if ( base_node<> * overlapped_child	=	child_to_unmount->get_next_overlapped() )
			{
				base_folder_node<> * old_parent	=	overlapped_child->get_parent();
				old_parent->unlink_child	(overlapped_child);
				overlapped_child->set_parent	(parent_to_traverse);
				new_children.push_back		(overlapped_child);
			}

			if ( m_args.submount_type == submount_type_hot_unmount )
				hot_unmount_node			(child_to_unmount, child_hash);
			else
				free_node					(m_file_system, child_to_unmount, m_args.root_write_lock, child_hash, m_args.allocator);
		}
		else
			new_children.push_back			(child);

		path.set_length						(saved_path_length);
		child							=	next_child;
	}

	parent_to_traverse->set_first_child		(new_children.front());
}

template <class Predicate> inline
void   unmounter::recursive_unmount_folder (fs_new::virtual_path_string &	path, 
										    u32 const						hash,
											Predicate const &				predicate,
										    base_folder_node<> *			parent_to_unmount)
{
	u32 saved_path_length				=	path.length();
	base_node<> * const overlapped_by_unmount	=	node_cast<base_node>(parent_to_unmount)->get_next_overlapped();
	base_folder_node<> * overlapped_by_unmount_folder	=	
		(overlapped_by_unmount && !overlapped_by_unmount->is_link()) ? 
		node_cast<base_folder_node>(overlapped_by_unmount) : NULL;	

	for ( base_node<> * child			=	parent_to_unmount->get_first_child();
						child; )
	{
		base_node<> * const next_child	=	child->get_next();

		u32 child_hash					=	fs_new::path_crc32(fs_new::virtual_path_string(child->get_name()), hash);
		fs_new::virtual_path_string & child_path	=	path;
		child_path.append_path				(child->get_name());
		
		base_node<> * child_to_unmount	=	NULL;
		if ( predicate(child) )
		{
			base_node<> * overlap_of_child_to_unmount	=	NULL;
			recursive_unmount_node			(child_path, child_hash, predicate, child_to_unmount, overlap_of_child_to_unmount);
			R_ASSERT						(child_to_unmount == child);
			if ( overlap_of_child_to_unmount )
				overlap_of_child_to_unmount->set_next_overlapped(child_to_unmount->get_next_overlapped());
		}

		if ( !child_to_unmount )
		{
			R_ASSERT						(!overlapped_by_unmount_folder->find_child(child->get_name(), NULL));
			overlapped_by_unmount_folder->prepend_child	(child);
		}

		if ( child_to_unmount )
		{
			if ( m_args.submount_type == submount_type_hot_unmount )
				hot_unmount_node			(child_to_unmount, child_hash);
			else
				free_node					(m_file_system, child_to_unmount, m_args.root_write_lock, child_hash, m_args.allocator);
		}

		path.set_length						(saved_path_length);
		child							=	next_child;
	}

	parent_to_unmount->set_first_child		(NULL);
}

template <class Predicate> inline
void   unmounter::recursive_unmount_node (fs_new::virtual_path_string & path, 
										  u32 const						hash,
										  Predicate &					predicate,
										  base_node<> * &				node_to_unmount,
										  base_node<> * &				overlap_of_node_to_unmount)
{
	node_to_unmount						=	NULL;
	base_node<> *	first_to_unmount	=	NULL;
	base_node<> *	last_to_unmount		=	NULL;
	base_node<> *	next_to_last		=	NULL;
	find_range_to_unmount					(path, hash, predicate, first_to_unmount, last_to_unmount, next_to_last);
	if ( !last_to_unmount )
		return;

	if ( last_to_unmount->is_file() )
	{
		base_node<> * const after_last	=	last_to_unmount->get_next_overlapped();
		// join folders if a file in between is unmounting
		if ( next_to_last && 
			 next_to_last->is_folder() && 
			 after_last && 
			 after_last->is_folder() )
		{
			transfer_children				(m_hashset, path, hash, first_to_unmount, after_last);
		}
	}
	else
	{
		recursive_unmount_folder_range		(path, hash, predicate, first_to_unmount, last_to_unmount);
	}

	overlap_of_node_to_unmount			=	next_to_last;
	node_to_unmount						=	last_to_unmount;
}

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_UNMOUNT_INLINE_H_INCLUDED