////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_helper.h"
#include "fs_impl_saving.h"
#include "fs_impl_saving_helper.h"
#include "fs_impl_saving_info_tree.h"

namespace xray {
namespace fs {

template <platform_pointer_enum target_pointer_size>
static fat_node<target_pointer_size> *   create_hard_link_node (saving_environment &							env, 
																u32 *											out_node_size,
																fat_db_hard_link_node<target_pointer_size> *	out_hard_link,
																fat_node_info *									duplicate)
{
	new ( out_hard_link )					fat_db_hard_link_node<target_pointer_size>;

		fat_node<target_pointer_size> *	const referenced_node	=	(fat_node<target_pointer_size> *)duplicate->saved_node;
		u32 const reference_offset		=	(u32)((pbyte)referenced_node - (pbyte)env.dest_buffer);

	out_hard_link->m_reference_node		=	(fat_node<target_pointer_size> *)reference_offset;
	* out_node_size						=	sizeof(fat_db_hard_link_node<target_pointer_size>);
	return									out_hard_link->cast_node();
}

template <platform_pointer_enum target_pointer_size>
static fat_node<target_pointer_size> *   create_soft_link_node (u32 *											out_node_size,
										 						fat_db_soft_link_node<target_pointer_size> *	out_soft_link,
										 						path_string *									out_reference_relative_path,
										 						fat_node<> * 									node, 
										 						fat_node_info *									duplicate)
{
	new ( out_soft_link )					fat_db_soft_link_node<target_pointer_size>;

		path_string							current_full_path, reference_full_path;
		node->get_full_path					(current_full_path);
		duplicate->node->get_full_path		(reference_full_path);

	make_relative_path						(out_reference_relative_path, current_full_path.c_str(), reference_full_path.c_str());
	* out_node_size						=	sizeof(fat_db_soft_link_node<target_pointer_size>);

	return									out_soft_link->cast_node();
}

template <platform_pointer_enum target_pointer_size>
static fat_node<target_pointer_size> *   create_inline_compressed_node (u32 *				 out_node_size,
																		fat_db_inline_compressed_file_node<target_pointer_size> * out_inline_compressed_node,
																		fat_node<> * 		 node, 
																		u32					 compressed_size,
																		file_size_type		 pos_in_db_with_offset)
{
	new ( out_inline_compressed_node )		fat_db_inline_compressed_file_node<target_pointer_size>;
	out_inline_compressed_node->m_uncompressed_size		=	node->get_raw_file_size();
	out_inline_compressed_node->m_size	=	compressed_size;
	out_inline_compressed_node->m_pos	=	(typename fat_db_inline_compressed_file_node<target_pointer_size>::file_size_type)pos_in_db_with_offset;
	* out_node_size						=	sizeof(fat_db_inline_compressed_file_node<target_pointer_size>);
	return									out_inline_compressed_node->cast_node();
}

template <platform_pointer_enum target_pointer_size>
static fat_node<target_pointer_size> *   create_inline_node (u32 *				  	out_node_size,
															 fat_db_inline_file_node<target_pointer_size> * out_inline_node,
															 fat_node<> * 			node, 
															 file_size_type			pos_in_db_with_offset)
{
	new ( out_inline_node )					fat_db_inline_file_node<target_pointer_size>;
	out_inline_node->m_size				=	node->get_raw_file_size();
	out_inline_node->m_pos				=	(typename fat_db_inline_file_node<target_pointer_size>::file_size_type)pos_in_db_with_offset;
	* out_node_size						=	sizeof(fat_db_inline_file_node<target_pointer_size>);
	return									out_inline_node->cast_node();
}

template <platform_pointer_enum target_pointer_size>
static fat_node<target_pointer_size> *   create_compressed_node (u32 *					out_node_size,
																 fat_db_compressed_file_node<target_pointer_size> * out_compressed_node,
																 fat_node<> * 			node, 
																 u32					compressed_size,
																 file_size_type			pos_in_db_with_offset)
{
	new ( out_compressed_node )				fat_db_compressed_file_node<target_pointer_size>;
	out_compressed_node->m_uncompressed_size	=	node->get_raw_file_size();
	out_compressed_node->m_size			=	compressed_size;
	out_compressed_node->m_pos			=	(typename fat_db_compressed_file_node<target_pointer_size>::file_size_type)pos_in_db_with_offset;
	* out_node_size						=	sizeof(fat_db_compressed_file_node<target_pointer_size>);
	return									out_compressed_node->cast_node();
}

template <platform_pointer_enum target_pointer_size>
static fat_node<target_pointer_size> *   create_file_node (u32 *				out_node_size,
													 	   fat_db_file_node<target_pointer_size> * out_node,
													 	   u32					raw_file_size,
													 	   file_size_type		pos_in_db_with_offset)
{
	new ( out_node )						fat_db_file_node<target_pointer_size>;
	out_node->m_size					=	raw_file_size;
	out_node->m_pos						=	(typename fat_db_file_node<target_pointer_size>::file_size_type)pos_in_db_with_offset;
	* out_node_size						=	sizeof(fat_db_file_node<target_pointer_size>);
	return									out_node->cast_node();
}

static bool   equal_disk_file_nodes	(saving_environment &	env,
									 fat_node<> * 			first, 
									 fat_node<> *			second)
{
	R_ASSERT								(!first->is_db() && !second->is_db());
	if ( first->get_raw_file_size() != second->get_raw_file_size() )
		return								false;

	path_string								first_path;
	env.fs->get_disk_path					(first, first_path);
	path_string								second_path;
	env.fs->get_disk_path					(second, second_path);
	R_ASSERT								(first_path != second_path);

	file_type * first_file				=	NULL;
	if ( !open_file(& first_file, open_file_read, first_path.c_str()) )
		return								false;

	file_type * second_file				=	NULL;
	if ( !open_file(& second_file, open_file_read, second_path.c_str()) )
	{
		close_file							(first_file);
		return								false;
	}

	bool const out_files_equal			=	files_equal(first_file, second_file, first->get_raw_file_size());
	close_file								(first_file);
	close_file								(second_file);
	return									out_files_equal;
}

static bool   equal_nodes (saving_environment & env, fat_node_info * first_info, fat_node_info * second_info);

static bool   equal_folder_nodes (saving_environment &	env,
								  fat_node_info *		first_info,
								  fat_node_info *		second_info)
{
	fat_node_info * it_child_of_first	=	first_info->first_child;
	fat_node_info * it_child_of_second	=	second_info->first_child;

	while ( it_child_of_first || it_child_of_second )
	{
		if ( !it_child_of_first || !it_child_of_second )
			return						false;

		if ( !equal_nodes(env, it_child_of_first, it_child_of_second) )
			return						false;

		it_child_of_first			=	it_child_of_first->next;
		it_child_of_second			=	it_child_of_second->next;
	}

	return								true;
}

static bool   equal_nodes (saving_environment &	env,
						   fat_node_info *		first_info,
						   fat_node_info *		second_info)
{
	if ( first_info->hash != second_info->hash )
		return								false;

	fat_node<> * const first_node		=	first_info->node;
	fat_node<> * const second_node		=	second_info->node;
	
	if ( first_node->is_folder() != second_node->is_folder() )
		return								false;

	if ( first_node->is_folder() )
		return								equal_folder_nodes		(env, first_info, second_info);

	return									equal_disk_file_nodes	(env, first_node, second_node);
}

fat_node_info *   save_db_helper::find_duplicate_folder (saving_environment &	env, fat_node_info * node_info)
{
	for (	fat_node_info_set::iterator	it		=	env.node_info_set->find(node_info->hash),
										end_it	=	env.node_info_set->end();
										it		!=	end_it;
										++it )
	{
		fat_node_info * const it_info		=	* it;

		if ( !it_info->node->is_folder() )
			continue;

		if ( !equal_folder_nodes(env, it_info, node_info) )
			continue;

		return								it_info;
	}

	return									NULL;
}

template <platform_pointer_enum target_pointer_size>
void   save_db_impl<target_pointer_size>::save_children_nodes   (saving_environment &					env, 
							 			   						fat_node<> *							node, 
										   						fat_node_info *							node_info,
										   						fat_folder_node<target_pointer_size> *	temp_folder,
										   						file_size_type							new_parent_offs)
{
	bool const		do_reverse_bytes	=	file_system::is_big_endian_format(env.db_format);
	fat_node<> *	child				=	node->get_first_child();
	fat_node_info * info_child			=	node_info->first_child;
	file_size_type	first_child_offs	=	u32(-1);
	file_size_type	child_prev_offs		=	NULL;
	fat_node<target_pointer_size> *	prev_fat_child	=	NULL;
	
	while ( child )
	{
		file_size_type	const saved_cur_offs	=	env.cur_offs;
		fat_node<target_pointer_size> * const saved_fat_child	=	
			save_nodes(env, child, info_child, child_prev_offs, new_parent_offs);

		child_prev_offs					=	saved_cur_offs + save_nodes_helper<target_pointer_size>::get_node_offs(saved_fat_child, do_reverse_bytes);
		if ( first_child_offs == u32(-1) )
			first_child_offs			=	child_prev_offs;

		if ( prev_fat_child )
		{
			prev_fat_child->m_next		=	(fat_node<target_pointer_size> *)((pbyte)saved_fat_child - (pbyte)env.dest_buffer);

			if ( do_reverse_bytes )
				reverse_bytes				(prev_fat_child->m_next);
		}

		child							=	child->get_next();
		info_child						=	info_child->next;

		if ( !child )
			saved_fat_child->m_next		=	NULL;

		prev_fat_child					=	saved_fat_child;
	}

	if ( first_child_offs == u32(-1) )
		first_child_offs				=	0;

	temp_folder->m_first_child			=	(fat_node<target_pointer_size> *)(size_t)first_child_offs;
}

template <platform_pointer_enum target_pointer_size>
fat_node<target_pointer_size> *   save_db_impl<target_pointer_size>::save_nodes  (saving_environment &	env, 
							 													 fat_node<> *			node, 
																				 fat_node_info *		node_info,
							 													 file_size_type 		prev_offs, 
							 													 file_size_type			parent_offs)
{
	R_ASSERT								(!node->is_db());

	bool const		is_sub_fat			=	!node_info->same_color(env.info_root);
	path_string								sub_fat_path;
	u32	sub_fat_size					=	0;
	if ( is_sub_fat )
		save_db_helper::save_sub_fat		(env, & sub_fat_path, & sub_fat_size, node, node_info);	

	bool const		do_reverse_bytes	=	file_system::is_big_endian_format(env.db_format);

	u32 const		name_len_with_zero	=	strings::length(node->get_name()) + 1;

	u32 const		max_node_class_size	=	math::max<u32>(	sizeof(fat_folder_node<target_pointer_size>), 
															sizeof(fat_db_compressed_file_node<target_pointer_size>),
															sizeof(fat_db_file_node<target_pointer_size>)	);

	u32				max_node_size		=	 max_node_class_size + name_len_with_zero + 16 + max_path_length;

	pstr			node_data			=	(pstr)ALLOCA(max_node_size);
	memory::zero							(node_data, max_node_size);
	
	fat_folder_node<target_pointer_size> * const					temp_folder					=	(fat_folder_node<target_pointer_size> *)					node_data;
	fat_db_file_node<target_pointer_size> * const					temp_file					=	(fat_db_file_node<target_pointer_size> *)					node_data;
	fat_db_compressed_file_node<target_pointer_size> * const		temp_compressed_file		=	(fat_db_compressed_file_node<target_pointer_size> *)		node_data;
	fat_db_inline_file_node<target_pointer_size> * const			temp_inline_file			=	(fat_db_inline_file_node<target_pointer_size> *)			node_data;
	fat_db_inline_compressed_file_node<target_pointer_size> * const	temp_inline_compressed_file	=	(fat_db_inline_compressed_file_node<target_pointer_size> *)	node_data;
	fat_db_soft_link_node<target_pointer_size> * const				temp_soft_link_node			=	(fat_db_soft_link_node<target_pointer_size> *)				node_data;
	fat_db_hard_link_node<target_pointer_size> * const				temp_hard_link_node			=	(fat_db_hard_link_node<target_pointer_size> *)				node_data;

	fat_node<target_pointer_size> *	temp =	NULL;
	bool			is_compressed		=	false;
	mutable_buffer	inlined_data;
	bool			is_inlined			=	false;

	path_string		reference_relative_path;
	u32				file_hash			=	0;
	u32				node_size			=	0;
	fat_node_info *	duplicate			=	NULL;

	if ( node->is_folder() && !is_sub_fat )
	{
#pragma message(XRAY_TODO("turned off links"))
		duplicate						=	NULL;
		//duplicate						=	save_db_helper::find_duplicate_folder(env, node_info);

		if ( !duplicate )
		{
			new ( temp_folder )				fat_folder_node<target_pointer_size>;
			temp						=	temp_folder->cast_node();
			node_size					=	sizeof(fat_folder_node<target_pointer_size>);
			// adding helper for folder to hash_set
			env.node_info_set->insert		(node_info->hash, node_info);
		}
	}
	else
	{
		file_size_type	pos_in_db_with_offset	=	0;
		u32				compressed_size	=	0;
		save_db_helper::save_node_file	(env, & duplicate, & pos_in_db_with_offset, & is_compressed, & inlined_data, 
										 & compressed_size, & file_hash, node, node_info, 
										 sub_fat_path.c_str(), sub_fat_size);

		is_inlined						=	!!inlined_data.size();

//#pragma message(XRAY_TODO("turned off links"))
		//bool const equal_names_with_duplicate_node	=	false;
		bool const equal_names_with_duplicate_node	=	
						duplicate && strings::equal(duplicate->node->get_name(), node->get_name());

		if ( !xray::identity(equal_names_with_duplicate_node) )
			duplicate						=	NULL; // no linking

		if ( xray::identity(equal_names_with_duplicate_node) )
		{
			; // will go with link
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
															  is_sub_fat ? sub_fat_size : node->get_raw_file_size(),
															  pos_in_db_with_offset);
		}
	}

	if ( duplicate )
	{
		if ( node_info->same_color(duplicate) )
			temp						=	create_hard_link_node(env, & node_size, temp_hard_link_node, duplicate);
		else
			temp						=	create_soft_link_node(& node_size, temp_soft_link_node, 
																  & reference_relative_path, node, duplicate);
	}
	bool const is_soft_link				=	temp->is_soft_link();
	bool const is_hard_link				=	temp->is_hard_link();
	bool const is_link					=	is_soft_link || is_hard_link;

	u32 node_alignment					=	8;
	if ( env.db_format != file_system::db_target_platform_pc )
	{
		if ( !is_inlined )
			node_alignment				=	4;
	}

	node_size							+=	name_len_with_zero;
	if ( reference_relative_path.length() )
		node_size						+=	reference_relative_path.length() + 1;

	u32 const padded_size				=	math::align_up(node_size, node_alignment);
	u32 const padded_size_with_inlined	=	is_inlined ? 
												math::align_up(padded_size + inlined_data.size(), node_alignment) 
												: 
												padded_size;

	if ( is_sub_fat )
		temp->m_flags					|=	file_system::is_sub_fat;
	else
		temp->m_flags					=	node->m_flags;

	strings::copy							(temp->m_name, name_len_with_zero, node->m_name);
	if ( reference_relative_path.length() )
	{
		size_t const offset_from_node_data	=	((pstr)temp->m_name - node_data) + name_len_with_zero;
		temp_soft_link_node->m_relative_path_to_referenced	=	(pstr)(env.cur_offs + offset_from_node_data);
		strings::copy						(temp->m_name + name_len_with_zero, 
											 reference_relative_path.length() + 1, 
											 reference_relative_path.c_str());
	}

	temp->m_flags						&=	~file_system::is_disk;
	temp->m_flags						|=	file_system::is_db;
	if ( is_compressed )
		temp->m_flags					|=	file_system::is_compressed;
	if ( is_inlined )
		temp->m_flags					|=	file_system::is_inlined;
	if ( is_soft_link )
		temp->m_flags					|=	file_system::is_soft_link;
	if ( is_hard_link )
		temp->m_flags					|=	file_system::is_hard_link;

	if ( !temp->is_folder() && !duplicate )
		temp->set_hash						(file_hash);

	if ( is_inlined )
		temp->set_inline_data				(memory::buffer((pbyte)(env.cur_offs + padded_size), inlined_data.size()));

	temp->set_next_overlapped				(NULL);

	pvoid const current_node_dest_pos	=	env.dest_buffer + env.cur_offs;
	file_size_type const current_node_dest_size	=	env.dest_buffer_size - env.cur_offs;
	R_ASSERT_CMP							(current_node_dest_size, >=, padded_size_with_inlined);

	if ( !duplicate )
		node_info->saved_node			=	(pbyte)current_node_dest_pos + ((pbyte)temp - (pbyte)node_data);

	temp->m_prev						=	node->m_prev	? (fat_node<target_pointer_size> *)(size_t) prev_offs	:	NULL;
	temp->m_parent						=	node->m_parent	? (fat_node<target_pointer_size> *)(size_t) parent_offs	:	NULL;

	file_size_type const new_parent_offs	=	env.cur_offs + save_nodes_helper<target_pointer_size>::get_node_offs(temp, false);
	env.cur_offs						+=	padded_size_with_inlined;
		
	if ( temp->is_folder() && !is_link ) 
		save_children_nodes					(env, node, node_info, temp_folder, new_parent_offs);

	u32 const non_reversed_flags		=	temp->get_flags(); // must be before reverse_bytes
	if ( do_reverse_bytes )
		temp->reverse_bytes_for_real_class	();

	memory::copy							(current_node_dest_pos, (size_t)current_node_dest_size, node_data, padded_size);

	if ( is_inlined )
	{
		pstr const inline_pos			=	(pstr)current_node_dest_pos + padded_size;
		size_t const buffer_left		=	(size_t)(current_node_dest_size - padded_size);
		memory::copy						(inline_pos, buffer_left, inlined_data.c_ptr(), inlined_data.size());
		size_t const pad_size_for_inlined	=	padded_size_with_inlined - padded_size - inlined_data.size();
		memory::zero						(inline_pos + inlined_data.size(), pad_size_for_inlined);
		pvoid data						=	inlined_data.c_ptr();
		XRAY_FREE_IMPL						(env.alloc, data);
	}

	++env.node_index;
	if ( env.callback )
		env.callback						(env.node_index, env.num_nodes, temp->get_name(), non_reversed_flags);

	return									(fat_node<target_pointer_size> *)
											((pbyte)current_node_dest_pos + save_nodes_helper<target_pointer_size>::get_node_offs(temp, do_reverse_bytes));
}

template class save_db_impl			<platform_pointer_32bit>;
template class save_db_impl			<platform_pointer_64bit>;

} // namespace fs
} // namespace xray