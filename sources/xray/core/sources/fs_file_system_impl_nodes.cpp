////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>
#include "fs_path_iterator.h"
#include "resources_manager.h"
#include "resources_managed_allocator.h"

namespace xray {
namespace fs   {

fat_folder_node<> *   file_system_impl::create_folder_node (u32 hash, pcstr name, u32 const name_len)
{
	fat_folder_node<> *		new_node = (fat_folder_node<> *)
							FS_ALLOC(char, sizeof(fat_folder_node<>) + name_len + 2);
	new	(new_node)			fat_folder_node<>;

	fat_node<>*	base	=	new_node->cast_node();

	base->m_flags		=	file_system::is_folder;
	strings::copy				(base->m_name, name_len+1, name);

	m_hash_set.insert		(hash, base);
	return					new_node;
}

fat_disk_folder_node<> *  file_system_impl::create_disk_folder_node (u32		hash, 
																	  pcstr		name, 
																	  u32 const	name_len,
																	  pcstr		full_name, 
																	  u32 const	full_name_len)
{
	ASSERT						(name_len);
	u32 const pad_with_byte	=	(name_len == 1);

	u32 const name_allocated_size	=	watcher_enabled() ? path_string::fixed_size : (name_len + 1);
	u32 const node_size		=	sizeof(fat_disk_folder_node<>) + name_allocated_size + pad_with_byte;
	u32 const disk_path_allocated_size	=	watcher_enabled() ? path_string::fixed_size : (full_name_len + 1);
	fat_disk_folder_node<> *	new_node = (fat_disk_folder_node<> *)
								FS_ALLOC(char, node_size + disk_path_allocated_size);
	new	(new_node)				fat_disk_folder_node<>;

	new_node->m_disk_path	=	(pstr)new_node + node_size;
	strings::copy				(new_node->m_disk_path, disk_path_allocated_size, full_name);

	fat_node<> * base	=	new_node->cast_node();

	base->m_flags		=	file_system::is_folder | file_system::is_disk;
	strings::copy			(base->m_name, name_allocated_size, name);

	m_hash_set.insert		(hash, base);
	return					new_node;
}

fat_disk_file_node<> *   file_system_impl::create_disk_file_node (u32		const					hash, 
 												 				   pcstr	const					name, 
												 				   u32		const					name_len, 
												 				   pcstr	const					full_name, 
												 				   u32		const					full_name_len,
																   u32 		const					file_size,
																   memory::base_allocator * const	allocator,
																   bool		const					add_to_hash_table)
{
	ASSERT						(name_len);
	u32 const pad_with_byte	=	(name_len == 1);

	u32 const name_allocated_size		=	watcher_enabled() ? path_string::fixed_size : (name_len + 1);
	u32 const node_size		=	sizeof(fat_disk_file_node<>) + name_allocated_size + pad_with_byte;
	u32 const disk_path_allocated_size	=	watcher_enabled() ? path_string::fixed_size : (full_name_len + 1);
	fat_disk_file_node<> *	new_node =	(fat_disk_file_node<>*) 
								XRAY_ALLOC_IMPL(*allocator, char, node_size + disk_path_allocated_size);

	new	(new_node)				fat_disk_file_node<>;

	new_node->m_disk_path	=	(pstr)new_node + node_size;
	new_node->m_size		=	file_size;
	strings::copy				(new_node->m_disk_path, disk_path_allocated_size, full_name);

	fat_node<> *	base	=	new_node->cast_node();
	base->m_flags			=	file_system::is_disk;
	strings::copy				(base->m_name, name_allocated_size, name);

	if ( add_to_hash_table )
		m_hash_set.insert		(hash, base);

	return						new_node;
}

void   file_system_impl::destroy_temp_disk_node (memory::base_allocator* allocator, fat_disk_file_node<> * node)
{
	ASSERT						(node->cast_node()->is_disk() && !node->cast_node()->is_folder());
	XRAY_FREE_IMPL				(* allocator, node);
}

void   file_system_impl::create_root ()
{
	ASSERT						(!m_root);
	m_root					=	create_folder_node(crc32("", 0), "", 0);
}

fat_node<> *   file_system_impl::find_node (pcstr const path_str, u32 * const out_hash) const
{
	verify_path_is_portable					(path_str);
	u32	const hash						=	path_crc32(path_str, strings::length(path_str));
	if ( out_hash )
		* out_hash						=	hash;

	node_hash_set::iterator	it			=	m_hash_set.find(hash);
	fat_node<> * candidate				=	( it != m_hash_set.end() ) ? *it : NULL;

	fat_node<> const * root_node		=	m_root->cast_node();
	while ( candidate )
	{
		reverse_path_iterator it		=	path_str;
		reverse_path_iterator end_it	=	reverse_path_iterator::end();

		fat_node<> *	cur				=	candidate;
		while ( it != end_it )
		{
			if ( !it.equal (cur->m_name) )
				break;

			cur							=	cur->m_parent;
			++it;
			if ( !cur )
				break;	
		}

		if ( it == end_it && cur == root_node )
			return							candidate;	

		candidate						=	candidate->get_next_hash_node();
	}

	return									NULL;
}

fat_folder_node<>*   file_system_impl::find_or_create_folder (fat_folder_node<> *	parent, 
															   pcstr				name, 
															   u32 const			name_len, 
															   u32 const			hash)
{
	for ( fat_node<> *	cur=parent->get_first_child(); 
						cur; 
						cur=cur->m_next )
	{
		if ( strings::equal(cur->m_name, name) )
		{
			if ( cur->is_folder() )
				return cur->cast_folder();

			parent->unlink_child	(cur);
			break;
		}
	}

	fat_folder_node<> * new_node	=	create_folder_node(hash, name, name_len);
	parent->prepend_child				(new_node->cast_node());
	return								new_node;
}

fat_folder_node<> *	 file_system_impl::find_or_create_folder (pcstr const path_str, u32 * out_hash)
{
	fat_node<> * work_node	=	find_node(path_str, out_hash);
	if ( work_node )
	{
		if ( work_node->is_folder() )
			return				work_node->cast_folder();

		fat_folder_node<> * parent	=	work_node->m_parent->cast_folder();
		ASSERT					(parent);
		parent->unlink_child	(work_node);
	}

	path_iterator it		=	path_str;
	path_iterator end_it	=	path_iterator::end();

	path_string	  name;

	fat_folder_node<> * folder	=	m_root;

	* out_hash				=	crc32("", 0);

	while ( it != end_it )
	{
		it.to_string			(name);
		* out_hash			=	crc32(name.c_str(), name.length(), * out_hash);

		folder				=	find_or_create_folder(folder, name.c_str(), name.length(), * out_hash);
		++it;
	}
	
	return						folder;
}

void   file_system_impl::free_node (fat_node<> * work_node, bool free_children)
{
	if ( free_children )
	{
		fat_node<> * child						=	work_node->get_first_child();
		while ( child )
		{
			fat_node<> * next					=	child->get_next();
			free_node								(child, free_children);
			child								=	next;
		}
	}
	
	R_ASSERT										(work_node->is_folder() ||
													 !work_node->is_associated(),
													"omg! trying to free node, while it's used by "
													"resources manager!");

	if ( !work_node->is_db() )
	{
		if ( !work_node->is_folder() )
		{
			fat_disk_file_node<> *	disk_file		=	work_node->cast_disk_file();
			disk_file->~fat_disk_file_node			();
			FS_FREE									(disk_file);
		}
		else
		{
			ASSERT									( work_node->is_folder() );
			if ( work_node->is_disk() )
			{
				fat_disk_folder_node<> *	folder	=	work_node->cast_disk_folder();
				folder->~fat_disk_folder_node		();
				FS_FREE								(folder);
			}
			else
			{
				fat_folder_node<> *		folder	=	work_node->cast_folder();
				folder->~fat_folder_node			();
				FS_FREE								(folder);
			}
		}
	}
}

} // namespace fs 
} // namespace xray 
