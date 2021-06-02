////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_SAVING_CREATING_NODES_H_INCLUDED
#define XRAY_VFS_SAVING_CREATING_NODES_H_INCLUDED

#include "pch.h"
#include "saving_environment.h"
#include <xray/fs/path_string_utils.h>

namespace xray {
namespace vfs {

template <platform_pointer_enum T>
inline base_node<T> *   create_hard_link_node (saving_environment &		env, 
											   u32 *					out_node_size,
											   hard_link_node<T> *		out_hard_link,
											   fat_node_info *			duplicate)
{
	new ( out_hard_link )					hard_link_node<T>;

		base_node<T> *	const referenced_node	=	(base_node<T> *)duplicate->saved_node;
		u32 const reference_offset		=	(u32)((pbyte)referenced_node - (pbyte)env.dest_buffer);

	out_hard_link->set_referenced_node		((base_node<T> *)reference_offset);
	* out_node_size						=	sizeof(hard_link_node<T>);
	return									node_cast<base_node>(out_hard_link);
}

template <platform_pointer_enum T>
inline base_node<T> *   create_soft_link_node  (u32 *					out_node_size,
										 		soft_link_node<T> *		out_soft_link,
												fs_new::virtual_path_string *	out_reference_relative_path,
										 		base_node<> * 			node, 
										 		fat_node_info *			duplicate)
{
	new ( out_soft_link )					soft_link_node<T>;

		fs_new::virtual_path_string			current_full_path, reference_full_path;
		node->get_full_path					(& current_full_path);
		duplicate->node->get_full_path		(& reference_full_path);

	fs_new::convert_to_relative_path		(out_reference_relative_path, reference_full_path, current_full_path);
	* out_node_size						=	sizeof(soft_link_node<T>);

	return									node_cast<base_node>(out_soft_link);
}

template <platform_pointer_enum T>
inline base_node<T> *   create_external_node   (u32 * out_node_size, external_subfat_node<T> * out_external_node, u32 archive_part_fat_size)
{
	new ( out_external_node )				external_subfat_node<T>;

	* out_node_size						=	sizeof(external_subfat_node<T>);
	out_external_node->external_fat_size	=	archive_part_fat_size;

	return									node_cast<base_node>(out_external_node);
}

template <platform_pointer_enum T>
inline base_node<T> *   create_erased_node   (u32 * out_node_size, erased_node<T> * out_erased_node)
{
	new ( out_erased_node )					erased_node<T>;

	* out_node_size						=	sizeof(erased_node<T>);

	base_node<T> * base					=	node_cast<base_node>(out_erased_node);	
	base->add_flags							(vfs_node_is_archive);
	return									base;
}

template <platform_pointer_enum T>
inline base_node<T> *   create_inline_compressed_node  (u32 *				 out_node_size,
														archive_inline_compressed_file_node<T> * out_node,
														base_node<> * 		 node, 
														u32					 compressed_size,
														file_size_type		 pos_in_db_with_offset)
{
	new ( out_node )	archive_inline_compressed_file_node<T>;
	out_node->set_uncompressed_size	(get_raw_file_size(node));
	out_node->set_size						(compressed_size);
	out_node->set_pos						((typename archive_inline_compressed_file_node<T>::file_size_type)
												pos_in_db_with_offset);

	* out_node_size						=	sizeof(archive_inline_compressed_file_node<T>);

	return									node_cast<base_node>(out_node);
}

template <platform_pointer_enum T>
inline base_node<T> *   create_inline_node (u32 *				  	out_node_size,
											archive_inline_file_node<T> * out_inline_node,
											base_node<> * 			node, 
											file_size_type			pos_in_db_with_offset)
{
	new ( out_inline_node )					archive_inline_file_node<T>;
	out_inline_node->set_size				(get_raw_file_size(node));
	out_inline_node->set_pos				((typename archive_inline_file_node<T>::file_size_type)pos_in_db_with_offset);
	* out_node_size						=	sizeof(archive_inline_file_node<T>);
	return									node_cast<base_node>(out_inline_node);
}

template <platform_pointer_enum T>
inline base_node<T> *   create_compressed_node (u32 *					out_node_size,
												archive_compressed_file_node<T> * out_compressed_node,
												base_node<> * 			node, 
												u32						compressed_size,
												file_size_type			pos_in_db_with_offset)
{
	new ( out_compressed_node )				archive_compressed_file_node<T>;
	out_compressed_node->set_uncompressed_size	(get_raw_file_size(node));
	out_compressed_node->set_size			(compressed_size);
	out_compressed_node->set_pos			((typename archive_compressed_file_node<T>::file_size_type)pos_in_db_with_offset);
	* out_node_size						=	sizeof(archive_compressed_file_node<T>);
	return									node_cast<base_node>(out_compressed_node);
}

template <platform_pointer_enum T>
inline base_node<T> *   create_file_node (u32 *						out_node_size,
									 	  archive_file_node<T> *	out_node,
									 	  u32						raw_file_size,
									 	  file_size_type			pos_in_db_with_offset)
{
	new ( out_node )						archive_file_node<T>;
	out_node->set_size						(raw_file_size);
	out_node->set_pos						((typename archive_file_node<T>::file_size_type)pos_in_db_with_offset);
	* out_node_size						=	sizeof(archive_file_node<T>);
	return									node_cast<base_node>(out_node);
}

template <platform_pointer_enum T>
inline base_node<T> *	create_folder_node	(u32 * out_node_size,
											 base_folder_node<T> * out_node)
{
	new ( out_node )						base_folder_node<T>;
	* out_node_size						=	sizeof(base_folder_node<T>);
	return									node_cast<base_node>(out_node);
}

template <platform_pointer_enum T>
inline base_node<T> *					create_archive_folder_mount_root_node
											(u32 *									out_node_size,
											 archive_folder_mount_root_node<T> *	out_node)
{
	new ( out_node )						archive_folder_mount_root_node<T>();
	* out_node_size						=	sizeof(archive_folder_mount_root_node<T>);

	return									node_cast<base_node>(out_node);
}

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_VFS_SAVING_CREATING_NODES_H_INCLUDED