////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include "fs_impl.h"
#include "fs_impl_saving.h"
#include "fs_impl_saving_info_tree.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// saving db/file_system
//-----------------------------------------------------------------------------------

bool   file_system_impl::save_db (saving_environment & env, fat_node_info * info_tree)
{
	threading::mutex_raii		raii	(m_mount_mutex);
	verify_path_is_portable				(env.logical_path.c_str());

	u32				hash;
	fat_node<> *	save_root		=	find_node_no_mount(env.logical_path.c_str(), & hash);
	if ( !save_root )
		return							false;

	bool const		fat_in_db		=	(env.db_file == env.fat_file);

	mutable_buffer						allocator_for_info;
	pvoid	allocator_for_info_base	=	NULL;
	bool const		root_save_db	=	!info_tree;
	if ( !info_tree )
	{
		u32 const num_nodes_in_fat	=	save_root->get_num_nodes();
		u32 const allocator_for_info_size	=	sizeof(fat_node_info) * num_nodes_in_fat;
		allocator_for_info_base		=	FS_ALLOC(char, allocator_for_info_size);
		allocator_for_info			=	mutable_buffer(allocator_for_info_base, allocator_for_info_size);
		info_tree					=	make_info_tree(* this, save_root, allocator_for_info);
	
		env.inline_data->begin_fitting	();
		calculate_sizes_for_info_tree	(save_root, info_tree, * env.inline_data);
		sort_info_tree					(save_root, info_tree);
		u32 color					=	1;
		paint_info_tree					(info_tree, env.fat_part_max_size, env.fat_part_max_size, color);
		env.inline_data->begin_fitting	();
	}

	u32 num_nodes					=	0;
	u32	const max_buffer_size		=	save_root->get_max_fat_size(* env.inline_data, info_tree, & num_nodes);

	file_size_type const db_file_origin	=	tell_file(env.db_file);

	if ( fat_in_db )
	{
		u32 const buffer_size_aligned	=	math::align_up (max_buffer_size + (u32)sizeof(fat_header), env.fat_alignment);
		seek_file						(env.db_file, buffer_size_aligned, SEEK_CUR);
	}

	pstr dest_buffer				=	FS_ALLOC(char, max_buffer_size);

	static fat_node_info_set			node_info_set;

	// writing nodes and filling file_system
	env.fs							=	this;
	env.cur_offs					=	0;
	env.dest_buffer					=	dest_buffer;
	env.dest_buffer_size			=	max_buffer_size;
	env.node_info_set				=	& node_info_set;
	env.node_index					=	0;
	env.num_nodes					=	num_nodes;
	env.info_root					=	info_tree;

	if ( env.db_format == file_system::db_target_platform_pc )
		save_db_impl<platform_pointer_64bit>::save_nodes	(env, save_root, info_tree, 0, 0);
	else
		save_db_impl<platform_pointer_32bit>::save_nodes	(env, save_root, info_tree, 0, 0);

	if ( fat_in_db )
		seek_file						(env.db_file, db_file_origin, SEEK_SET);

	// writing file_system

	u32 const	real_buffer_size	=	env.cur_offs;
	fat_header	header;
	header.num_nodes				=	num_nodes;
	header.buffer_size				=	(u32)real_buffer_size;
	env.result_fat_size				=	real_buffer_size + sizeof(fat_header);
	
	if ( file_system::is_big_endian_format(env.db_format) )
	{
		header.reverse_bytes			();
		header.set_big_endian			();
	}
	else
		header.set_little_endian		();
	
	write_file							(env.fat_file, & header, sizeof(fat_header));
	write_file							(env.fat_file, dest_buffer, (size_t)real_buffer_size);

	FS_FREE								(dest_buffer);

	if ( root_save_db )
		env.node_info_set->clear		();

	if ( allocator_for_info_base )
		FS_FREE							(allocator_for_info_base);

	return								true;
}

void   save_db_helper::save_sub_fat	  (saving_environment &		env, 
									   path_string *			out_sub_fat_path,
									   u32 *					out_sub_fat_size,
									   fat_node<> *				node, 
									   fat_node_info *			node_info)
{
	file_type *	sub_fat_file			=	NULL;

	directory_part_from_path				(out_sub_fat_path, env.fat_disk_path.c_str());
	static u32 temp_name				=	0;
	out_sub_fat_path->appendf				("/temp_%d.db", ++temp_name);

	bool const open_result				=	open_file(&sub_fat_file,	open_file_write | 
																		open_file_create | 
																		open_file_truncate, out_sub_fat_path->c_str());
	R_ASSERT_U								(open_result);

	saving_environment sub_fat_env		=	env;

	node->get_full_path						(sub_fat_env.logical_path);
	sub_fat_env.fat_file				=	sub_fat_file;
	sub_fat_env.db_file					=	sub_fat_file;
	sub_fat_env.info_root				=	node_info;
	file_size_type const pos_in_db		=	tell_file(env.db_file);
	sub_fat_env.offset_for_file_positions	+=	pos_in_db;

	bool const result					=	env.fs->save_db(sub_fat_env, node_info);
	R_ASSERT_U								(result);
	close_file								(sub_fat_file);

	* out_sub_fat_size					=	sub_fat_env.result_fat_size;
}

} // namespace fs 
} // namespace xray 
