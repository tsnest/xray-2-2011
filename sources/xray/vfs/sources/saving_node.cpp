////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_path_string.h>
#include "saving.h"
#include "saving_creating_nodes.h"
#include "saving_helper.h"

namespace xray {
namespace vfs {

template <platform_pointer_enum T>
void   set_hash							(base_node<T> * node, u32 hash)
{
	R_ASSERT								(!node->is_link());
	R_ASSERT								(!node->is_folder());
	R_ASSERT								(node->is_archive());

	if ( node->is_external_sub_fat() || node->is_erased() )
	{
		// thats ok... we dont do anything
	}
	else if ( node->is_inlined() )
	{
		if ( node->is_compressed() )
			node_cast<archive_inline_compressed_file_node>(node)->set_hash(hash);
		else
			node_cast<archive_inline_file_node>(node)->set_hash(hash);
	}
	else
	{
		if ( node->is_compressed() )
			node_cast<archive_compressed_file_node>(node)->set_hash(hash);
		else
			node_cast<archive_file_node>(node)->set_hash(hash);
	}
}

template <platform_pointer_enum T>
void   archive_saver::save_children_nodes (base_node<> *			node, 
										   fat_node_info *			node_info,
										   base_folder_node<T> *	temp_folder,
										   file_size_type			new_parent_offs)
{
	base_node<> *	child				=	node->get_first_child();
	fat_node_info * info_child			=	node_info->first_child;
	file_size_type	first_child_offs	=	file_size_type(-1);
	base_node<T> *	prev_fat_child		=	NULL;
	
	while ( child )
	{
		file_size_type	const saved_cur_offs	=	m_env.cur_offs;
		base_node<T> * const saved_fat_child	=	save_nodes<T>(child, info_child, new_parent_offs);

		if ( first_child_offs == file_size_type(-1) )
			first_child_offs			=	saved_cur_offs + save_nodes_helper<T>::get_node_offs(saved_fat_child, m_do_reverse_bytes);

		if ( prev_fat_child )
		{
			size_t const next_offs		=	(pbyte)saved_fat_child - (pbyte)m_env.dest_buffer;
			prev_fat_child->set_next		(file_size_type_to_platform_pointer<base_node<T>, T>
												(next_offs));

			if ( m_do_reverse_bytes )
				reverse_bytes				(prev_fat_child->m_next);
		}

		child							=	child->get_next();
		info_child						=	info_child->next;

		if ( !child )
			saved_fat_child->set_next		(NULL);

		prev_fat_child					=	saved_fat_child;
	}

	if ( first_child_offs == file_size_type(-1) )
		first_child_offs				=	0;

	temp_folder->set_first_child			(file_size_type_to_platform_pointer<base_node<T>, T>
												(first_child_offs));
}

template <platform_pointer_enum T>
base_node<T> *   archive_saver::save_nodes (base_node<> *			node, 
										    fat_node_info *			node_info,
							 			    file_size_type			parent_offs)
{
	bool const		is_mount_root		=	(m_env.cur_offs == 0);

	bool const		is_sub_fat			=	!node_info->same_color(m_env.info_root);
	bool const		is_archive_part		=	node_info->is_archive_part && (node != m_env.save_root);
	fs_new::native_path_string				sub_fat_path;
	fs_new::native_path_string				archive_part_path;
	u32	sub_fat_size					=	0;

	if ( is_archive_part )
		save_archive_part					(& archive_part_path, & sub_fat_size, node, node_info);	
	else if ( is_sub_fat )
		save_sub_fat						(& sub_fat_path, & sub_fat_size, node, node_info);	

	u32 const		name_len_with_zero	=	(m_root_save_db && is_mount_root) ? 
											sizeof(string_path)
												:
											strings::length(node->get_name()) + 1;

	u32 const		max_node_class_size	=	math::max<u32>(	sizeof(base_folder_node<T>), 
															sizeof(archive_compressed_file_node<T>),
															math::max<u32>(sizeof(archive_file_node<T>),
															sizeof(archive_folder_mount_root_node<T>)) );

	u32				max_node_size		=	 max_node_class_size + name_len_with_zero + 16 + fs_new::max_path_length;

	pstr			node_data			=	(pstr)ALLOCA(max_node_size);
	memory::zero							(node_data, max_node_size);
	
	base_folder_node<T> *					temp_folder					=	(base_folder_node<T> *)				node_data;
	archive_file_node<T> * const			temp_file					=	(archive_file_node<T> *)			node_data;
	archive_compressed_file_node<T> * const	temp_compressed_file		=	(archive_compressed_file_node<T> *)	node_data;
	archive_inline_file_node<T> * const		temp_inline_file			=	(archive_inline_file_node<T> *)		node_data;
	archive_inline_compressed_file_node<T> * const	temp_inline_compressed_file	=	(archive_inline_compressed_file_node<T> *)	node_data;
	soft_link_node<T> * const				temp_soft_link_node			=	(soft_link_node<T> *)				node_data;
	hard_link_node<T> * const				temp_hard_link_node			=	(hard_link_node<T> *)				node_data;
	archive_folder_mount_root_node<T> * const	temp_mount_root_node	=	(archive_folder_mount_root_node<T> *)	node_data;
	external_subfat_node<T> * const			temp_external_node			=	(external_subfat_node<T> *)				node_data;
	erased_node<T> * const					temp_erased_node			=	(erased_node<T> *)					node_data;

	base_node<T> *	temp				=	NULL;
	bool			is_compressed		=	false;
	mutable_buffer	inlined_data;
	bool			is_inlined			=	false;

	fs_new::virtual_path_string				soft_link_path;
	u32				file_hash			=	0;
	u32				node_size			=	0;
	fat_node_info *	duplicate			=	NULL;

	if ( node->is_folder() && !is_sub_fat )
	{
		if ( is_mount_root )
		{
			temp						=	create_archive_folder_mount_root_node(& node_size, temp_mount_root_node);
			temp_folder					=	node_cast<base_folder_node>(temp_mount_root_node);
		}
		else
		{			
			duplicate					=	(m_args.flags & save_flag_forbid_folder_links) ? 
												NULL : find_duplicate_folder(node_info);

			if ( !duplicate )
			{
				temp					=	create_folder_node<T>(& node_size, temp_folder);
				temp->add_flags				(vfs_node_is_archive);
				
				// adding helper for folder to hash_set
				if ( !saving_patch() )
					m_env.node_info_set->insert	(node_info->hash, node_info);
			}
		}
	}
	else if ( node->is_erased() )
	{
		temp							=	create_erased_node(& node_size, temp_erased_node);
	}
	else if ( is_archive_part )
	{
		temp							=	create_external_node(& node_size, temp_external_node, sub_fat_size);
	}
	else
	{
		file_size_type	pos_in_db_with_offset	=	0;
		u32				compressed_size	=	0;
		save_node_file						(& duplicate, & pos_in_db_with_offset, & is_compressed, & inlined_data, 
											 & compressed_size, & file_hash, node, node_info, 
											 sub_fat_path, sub_fat_size);

		is_inlined						=	!!inlined_data.size();

		bool const equal_names_with_duplicate_node	=	duplicate && 
														strings::equal(duplicate->node->get_name(), node->get_name());

		if ( !equal_names_with_duplicate_node )
			duplicate					=	NULL; // no linking

		if ( equal_names_with_duplicate_node )
		{
		}
		else if ( is_inlined )
		{
			if ( is_compressed )
				temp					=	create_inline_compressed_node(& node_size, temp_inline_compressed_file, 
																		  node, compressed_size, pos_in_db_with_offset);
			else
				temp					=	create_inline_node(& node_size, temp_inline_file, node, pos_in_db_with_offset);
		}
		else
		{
			if ( is_compressed )
				temp					=	create_compressed_node(& node_size, temp_compressed_file,
																   node, compressed_size, pos_in_db_with_offset);
			else
				temp					=	create_file_node (& node_size, temp_file, 
															  is_sub_fat ? sub_fat_size : get_raw_file_size(node),
															  pos_in_db_with_offset);
		}
	}

	if ( duplicate )
	{
		if ( node_info->same_color(duplicate) )
			temp						=	create_hard_link_node(m_env, & node_size, temp_hard_link_node, duplicate);
		else
			temp						=	create_soft_link_node(& node_size, temp_soft_link_node, 
				 												  & soft_link_path, node, duplicate);
		temp->add_flags						(vfs_node_is_archive);
	}

	bool const is_soft_link				=	temp->is_soft_link();
	bool const is_hard_link				=	temp->is_hard_link();
	bool const is_link					=	is_soft_link || is_hard_link;

	u32 node_alignment					=	8;
	if ( m_args.archive_platform != archive_platform_pc )
	{
		if ( !is_inlined )
			node_alignment				=	4;
	}

	node_size							+=	name_len_with_zero;
	if ( soft_link_path.length() )
		node_size						+=	soft_link_path.length() + 1;
	if ( is_archive_part )
		node_size						+=	archive_part_path.length() + 1;

	u32 const padded_size				=	math::align_up(node_size, node_alignment);
	u32 const padded_size_with_inlined	=	is_inlined ? 
												math::align_up(padded_size + inlined_data.size(), node_alignment) 
												: 
												padded_size;

	if ( is_sub_fat )
		temp->add_flags						(vfs_node_is_sub_fat);

	strings::copy							(temp->m_name, name_len_with_zero, node->m_name);
	if ( soft_link_path.length() )
	{
		size_t const offset_from_node_data	=	((pstr)temp->m_name - node_data) + name_len_with_zero;
		temp_soft_link_node->relative_path	=	(pstr)(m_env.cur_offs + offset_from_node_data);
		strings::copy						(temp->m_name + name_len_with_zero, 
											 soft_link_path.length() + 1, 
											 soft_link_path.c_str());
	}
	else if ( is_archive_part )
	{
		size_t const offset_from_node_data	=	((pstr)temp->m_name - node_data) + name_len_with_zero;
		temp_external_node->relative_path_to_external	=	(pstr)(m_env.cur_offs + offset_from_node_data);
		strings::copy						(temp->m_name + name_len_with_zero, 
											 archive_part_path.length() + 1, 
											 archive_part_path.c_str());
	}

	R_ASSERT								(!temp->has_flags(vfs_node_is_physical));
	R_ASSERT								(temp->has_flags(vfs_node_is_archive));

	if ( is_compressed )					R_ASSERT(temp->is_compressed());
	if ( is_inlined )						R_ASSERT(temp->is_inlined());
	if ( is_soft_link )						R_ASSERT(temp->is_soft_link());
	if ( is_hard_link )						R_ASSERT(temp->is_hard_link());
	if ( is_archive_part )					R_ASSERT(temp->is_external_sub_fat());

	if ( !temp->is_folder() && !duplicate )
		set_hash							(temp, file_hash);

	if ( is_inlined )
		set_inline_data						(temp, memory::buffer((pbyte)(m_env.cur_offs + padded_size), inlined_data.size()));

	temp->set_next_overlapped				(NULL);
	temp->set_hashset_next					(NULL);

	if ( temp->is_mount_root() )
	{
		temp->set_mount_helper_parent		(NULL);
		temp_mount_root_node->node		=	file_size_type_to_platform_pointer<base_node<T>, T>
												(m_env.cur_offs + save_nodes_helper<T>::get_node_offs(temp, m_do_reverse_bytes));
		m_mount_root_offs				=	m_env.cur_offs + 
												(u32)(mount_root_node_base<T> *)(archive_folder_mount_root_node<T> *)(NULL);
			
			///save_nodes_helper<T>::get_mount_root_node_base_offs<archive_folder_mount_root_node<T> >();
	}
	else
	{
		temp->set_mount_root				(file_size_type_to_platform_pointer_pod<mount_root_node_base<T>, T>
												(m_mount_root_offs));		
	}

	pvoid const current_node_dest_pos	=	m_env.dest_buffer + m_env.cur_offs;
	file_size_type const current_node_dest_size	=	m_env.dest_buffer_size - m_env.cur_offs;
	R_ASSERT_CMP							(current_node_dest_size, >=, padded_size_with_inlined);

	if ( !duplicate )
		node_info->saved_node			=	(pbyte)current_node_dest_pos + ((pbyte)temp - (pbyte)node_data);

	temp->set_parent						(file_size_type_to_platform_pointer<base_folder_node<T>, T>
												(node->m_parent	? parent_offs : 0));

	u32 const folder_offs_in_mount_root	=	(u32) & ((archive_folder_mount_root_node<T> *)(NULL))->folder;
	file_size_type const new_parent_offs	=	m_env.cur_offs + is_mount_root ? folder_offs_in_mount_root : 0;
	m_env.cur_offs						+=	padded_size_with_inlined;
		
	if ( temp->is_folder() && !is_link ) 
		save_children_nodes					(node, node_info, temp_folder, new_parent_offs);

	u32 const non_reversed_flags		=	temp->get_flags(); // must be before reverse_bytes
	if ( m_do_reverse_bytes )
		temp->reverse_bytes_for_final_class	(reverse_direction_from_native);

	memory::copy							(current_node_dest_pos, (size_t)current_node_dest_size, node_data, padded_size);

	if ( is_inlined )
	{
		pstr const inline_pos			=	(pstr)current_node_dest_pos + padded_size;
		file_size_type const buffer_left	=	current_node_dest_size - padded_size;
		memory::copy						(inline_pos, (size_t)buffer_left, inlined_data.c_ptr(), inlined_data.size());
		file_size_type const pad_size_for_inlined	=	padded_size_with_inlined - padded_size - inlined_data.size();
		memory::zero						(inline_pos + inlined_data.size(), (size_t)pad_size_for_inlined);
		pvoid data						=	inlined_data.c_ptr();
		XRAY_FREE_IMPL						(m_args.allocator, data);
	}

	if ( !is_sub_fat )
	{
		++m_env.node_index;
		if ( m_args.callback )
			m_args.callback					(m_env.node_index, m_env.num_nodes, temp->get_name(), non_reversed_flags);
	}

	return									(base_node<T> *)
											((pbyte)current_node_dest_pos + save_nodes_helper<T>::get_node_offs(temp, m_do_reverse_bytes));
}

void instantiator ()
{
	((archive_saver *)NULL)->save_nodes	<platform_pointer_32bit>	(NULL, NULL, 0);
	((archive_saver *)NULL)->save_nodes	<platform_pointer_64bit>	(NULL, NULL, 0);
}

} // namespace vfs
} // namespace xray