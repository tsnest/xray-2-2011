////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>
#include <xray/fs_path_iterator.h>

namespace xray {
namespace fs   {

fat_folder_node<> *   file_system_impl::create_folder_node (u32 hash, pcstr name, u32 const name_len)
{
	fat_folder_node<> *		new_node = (fat_folder_node<> *)
							FS_ALLOC(char, sizeof(fat_folder_node<>) + name_len + 2);
	new	(new_node)			fat_folder_node<>;

	fat_node<>*	base	=	new_node->cast_node();

	base->m_flags		=	file_system::is_folder;
	strings::copy			(base->m_name, name_len+1, name);

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

	u32 const name_allocated_size	=	is_watcher_enabled() ? path_string::fixed_size : (name_len + 1);
	u32 const node_size		=	sizeof(fat_disk_folder_node<>) + name_allocated_size + pad_with_byte;
	u32 const disk_path_allocated_size	=	is_watcher_enabled() ? path_string::fixed_size : (full_name_len + 1);
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

	u32 const name_allocated_size		=	is_watcher_enabled() ? path_string::fixed_size : (name_len + 1);
	u32 const node_size		=	sizeof(fat_disk_file_node<>) + name_allocated_size + pad_with_byte;
	u32 const disk_path_allocated_size	=	is_watcher_enabled() ? path_string::fixed_size : (full_name_len + 1);
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
	ASSERT									(node->cast_node()->is_disk() && !node->cast_node()->is_folder());
	XRAY_FREE_IMPL							(* allocator, node);
}

void   file_system_impl::create_root ()
{
	ASSERT									(!m_root);
	m_root								=	create_folder_node(crc32("", 0), "", 0);
}

fat_folder_node<>*   file_system_impl::find_or_create_folder (fat_folder_node<> *	parent, 
															  pcstr					name, 
															  u32 const				name_len, 
															  u32 const				hash)
{
	for ( fat_node<> *	cur = parent->get_first_child(); 
						cur; 
						cur = cur->m_next )
	{
		if ( strings::equal(cur->m_name, name) )
		{
			if ( cur->is_folder() )
				return					cur->cast_folder();

			parent->unlink_child		(cur, false);
			break;
		}
	}

	fat_folder_node<> * new_node	=	create_folder_node(hash, name, name_len);
	parent->prepend_child				(new_node->cast_node());
	return								new_node;
}

fat_folder_node<> *	 file_system_impl::find_or_create_folder (pcstr const path_str, u32 * out_hash)
{
	fat_node<> * work_node		=	find_node_no_mount(path_str, out_hash);
	if ( work_node )
	{
		if ( work_node->is_folder() )
			return					work_node->cast_folder();

		fat_folder_node<> * parent	=	work_node->m_parent->cast_folder();
		ASSERT						(parent);
		parent->unlink_child		(work_node, false);
	}

	path_iterator it			=	path_str;
	path_iterator end_it		=	path_iterator::end();

	path_string	  name;

	fat_folder_node<> * folder	=	m_root;

	* out_hash					=	crc32("", 0);

	while ( it != end_it )
	{
		it.to_string				(name);
		* out_hash				=	crc32(name.c_str(), name.length(), * out_hash);
		
		if ( fat_disk_folder_node<> * disk_folder = folder->cast_node()->cast_disk_folder() )
			if ( !disk_folder->is_scanned(false) )
				continue_mount_disk	(folder->cast_node(), NULL, file_system::recursive_false);
		
		folder					=	find_or_create_folder(folder, name.c_str(), name.length(), * out_hash);
		++it;
	}
	
	return							folder;
}

void   file_system_impl::free_node (fat_node<> * work_node, bool free_children)
{
	if ( free_children )
	{
		fat_node<> * child		=	work_node->get_first_child(false);
		while ( child )
		{
			fat_node<> * next	=	child->get_next(false);
			free_node				(child, free_children);
			child				=	next;
		}
	}

	////DO NOT COMMITE //////////////////////////////////////////////////////////////////////
	// 	R_ASSERT										(work_node->is_folder() ||
	// 													 !work_node->is_associated(),
	// 													"omg! trying to free node, while it's used by "
	// 													"resources manager!");
	//////////////////////////////////////////////////////////////////////////

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
