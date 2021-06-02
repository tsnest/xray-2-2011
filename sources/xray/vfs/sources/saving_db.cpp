////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/path_string_utils.h>
#include <xray/fs/device_utils.h>
#include <xray/vfs/virtual_file_system.h>
#include "fat_header.h"
#include "saving.h"
#include "saving_info_tree.h"
#include "branch_locks.h"

namespace xray {
namespace vfs   {

using namespace fs_new;

template <platform_pointer_enum T>
u32   get_max_fat_size_impl				(base_node<T> const *		node, 
										 fat_inline_data const &	inline_data, 
										 fat_node_info *			info,
										 fat_node_info *			root_info,
										 u32 *						out_nodes_count)
{
	R_ASSERT								(!node->is_link());
	if ( !info->same_color(root_info) )
		return								0;

	++ * out_nodes_count;

	u32 const max_node_size				=	(u32)math::max(	math::max(	sizeof(archive_file_node<T>),
																		sizeof(base_folder_node<T>) ),
																		sizeof(archive_inline_compressed_file_node<T>)	);
		
	u32 size							=	max_node_size;
	size								+=	strings::length(node->get_name()) + 1;
	size								+=	sizeof(pvoid);

	if ( !node->is_folder() )
	{	
		pcstr extension					=	extension_from_path(node->get_name());
		fat_inline_data::item const * item	=	NULL;
		if ( inline_data.find_by_extension(& item, extension) && (item->current_size == fat_inline_data::item::no_limit) )
		{
			size						+=	get_file_size(node);
			size						+=	XRAY_DEFAULT_ALIGN_SIZE; // may need alignment
		}
	}

	base_node<> const * cur_child		=	node->get_first_child();
	fat_node_info *	info_child			=	info->first_child;
	while ( cur_child )
	{
		size							+=	get_max_fat_size_impl(cur_child, inline_data, info_child, info_child, out_nodes_count);
		cur_child						=	cur_child->get_next();
		info_child						=	info_child->next;
	}

	return									size;
}

template <platform_pointer_enum T>
u32   get_max_fat_size					(base_node<T> const *		node, 
										 fat_inline_data const &	inline_data, 
										 fat_node_info *			info,
										 u32 *						out_nodes_count)
{
	R_ASSERT								(!node->is_link());
	R_ASSERT								(out_nodes_count);
	* out_nodes_count					=	0;
	u32 out_size						=	inline_data.total_size_for_extensions_with_limited_size();
	out_size							+=	sizeof(string_path);
	out_size							+=	sizeof(archive_folder_mount_root_node<T>);
	return									out_size + get_max_fat_size_impl(node, inline_data, info, info, out_nodes_count);
}

fat_node_info *   make_info_tree		(pvoid &						out_allocated_nodes_buffer, 
										 base_node<> *					root_node, 
										 synchronous_device_interface &	device,
										 memory::base_allocator *		allocator)
{
	u32 const count_of_nodes_in_fat		=	calculate_count_of_nodes(root_node);
	u32 const allocator_for_info_size	=	sizeof(fat_node_info) * count_of_nodes_in_fat;
	mutable_buffer							allocator_for_info;
	out_allocated_nodes_buffer			=	XRAY_ALLOC_IMPL(allocator, char, allocator_for_info_size);
	allocator_for_info					=	mutable_buffer(out_allocated_nodes_buffer, allocator_for_info_size);
	fat_node_info * const out_info_tree	=	make_info_tree(device, root_node, allocator_for_info);

	return									out_info_tree;
}

void   add_to_info_set					(fat_node_info_set * const	info_set, 
										 fat_node_info * const		node,
										 vfs_hashset *				from_hashset,
										 vfs_hashset *				patch_hashset)
{
	virtual_path_string						path;
	node->node->get_full_path				(& path);
	base_node<> * const from_node		=	from_hashset->find_no_lock(path.c_str());
	base_node<> * const patch_node		=	patch_hashset->find_no_lock(path.c_str());

	bool const can_link					=	!from_node || !from_node->is_link() || patch_node;

	if ( can_link )
		info_set->insert					(node->hash, node);

	node->set_color							(u32(-1));
	
	for ( fat_node_info *	it_child	=	node->first_child;
							it_child	!=	NULL;
							it_child	=	it_child->next	)
	{
		add_to_info_set						(info_set, it_child, from_hashset, patch_hashset);
	}
}

bool   archive_saver::save_db			()
{
	bool locked_save_root				=	false;
	if ( !m_env.save_root )
	{
		m_hashset.find_and_lock_branch		(m_env.save_root, m_env.virtual_path.c_str(), lock_type_read);
		if ( !m_env.save_root )
			return							false;
		locked_save_root				=	true;
	}

	static fat_node_info_set				node_info_set;

	bool const		fat_in_db			=	(m_env.db_file == m_env.fat_file);

	fat_node_info * info_tree			=	m_env.info_root;
	pvoid	info_buffer					=	NULL;
	
	m_root_save_db						=	!info_tree;
	if ( m_root_save_db )
	{
		m_env.temp_name_index			=	0;
		info_tree						=	make_info_tree(info_buffer, m_env.save_root, m_args.device, m_args.allocator);

		if ( saving_patch() )
		{
			R_ASSERT						(m_args.to_vfs);
			R_ASSERT						(m_args.to_vfs_info_tree);
			R_ASSERT						(m_args.from_vfs_info_tree);
			add_to_info_set					(& node_info_set, m_args.to_vfs_info_tree, & m_args.from_vfs->hashset, & m_hashset);
		}
	
		m_args.inline_data.begin_fitting	();
		calculate_sizes_for_info_tree		(m_env.save_root, info_tree, m_args.inline_data);
		sort_info_tree						(m_env.save_root, info_tree);
		u32 color						=	1;
		paint_subfats						(info_tree, m_args.fat_part_max_size, m_args.fat_part_max_size, color);
		m_env.archive_parts_count		=	split_into_archive_parts(info_tree, m_args.archive_part_max_size);
		m_args.inline_data.begin_fitting	();

		m_env.node_index				=	0;
	}

	m_do_reverse_bytes					=	is_big_endian_platform(m_args.archive_platform);

	u32 num_nodes						=	0;
	u32	const max_buffer_size			=	get_max_fat_size(m_env.save_root, m_args.inline_data, info_tree, & num_nodes);
	if ( m_root_save_db )
		m_env.num_nodes					=	num_nodes;

	file_size_type const db_file_origin	=	m_args.device->tell(m_env.db_file);

	if ( fat_in_db )
	{
		u32 const buffer_size_aligned	=	math::align_up (max_buffer_size + (u32)sizeof(fat_header), 
															m_args.fat_alignment);
		m_args.device->seek					(m_env.db_file, buffer_size_aligned, seek_file_current);
	}

	pstr dest_buffer					=	XRAY_ALLOC_IMPL(m_args.allocator, char, max_buffer_size);

	// writing nodes and filling file_system
	m_env.cur_offs						=	0;
	m_env.dest_buffer					=	dest_buffer;
	m_env.dest_buffer_size				=	max_buffer_size;
	m_env.node_info_set					=	& node_info_set;
	m_env.info_root						=	info_tree;

	if ( m_args.archive_platform == archive_platform_pc )
		save_nodes<platform_pointer_64bit>	(m_env.save_root, info_tree, 0);
	else
		save_nodes<platform_pointer_32bit>	(m_env.save_root, info_tree, 0);

	if ( fat_in_db )
		m_args.device->seek					(m_env.db_file, db_file_origin, seek_file_begin);

	// writing file_system

	u32 const	real_buffer_size		=	m_env.cur_offs;
	fat_header	header;
	header.num_nodes					=	num_nodes;
	header.buffer_size					=	(u32)real_buffer_size;
	m_env.result_fat_size				=	real_buffer_size + sizeof(fat_header);
	
	if ( is_big_endian_platform(m_args.archive_platform) )
	{
		header.reverse_bytes				();
		header.set_big_endian				();
	}
	else
		header.set_little_endian			();
	
	m_args.device->write					(m_env.fat_file, & header, sizeof(fat_header));
	m_args.device->write					(m_env.fat_file, dest_buffer, (size_t)real_buffer_size);

	XRAY_FREE_IMPL							(m_args.allocator, dest_buffer);

	if ( m_root_save_db )
		m_env.node_info_set->clear			();

	if ( info_buffer )
		XRAY_FREE_IMPL						(m_args.allocator, info_buffer);

	if ( locked_save_root )
		unlock_branch						(m_env.save_root, lock_type_read);

	return									true;
}

bool	create_archive_files			(native_path_string const & 		archive_path, 
										 native_path_string const & 		fat_path,
										 synchronous_device_interface	&	device,
										 file_type * &						out_db_file,
										 file_type * &						out_fat_file);

void	close_archive_files				(synchronous_device_interface	&	device,
										 file_type * &						db_file,
										 file_type * &						fat_file);

void   archive_saver::save_archive_part	(native_path_string *	out_archive_part_path,
										 u32 *					out_archive_fat_size,
										 base_node<> *			node, 
										 fat_node_info *		node_info)
{
	native_path_string						directory;
	get_path_without_last_item				(& directory, m_args.fat_path.c_str());
	native_path_string						file_name;
	file_name_with_no_extension_from_path	(& file_name, m_args.fat_path.c_str());
	native_path_string	const extension	=	extension_from_path(m_args.fat_path.c_str());
	
	out_archive_part_path->assignf			("%s.%d-%d.%s", file_name.c_str(), 
											 m_env.next_archive_part, m_env.archive_parts_count, 
											 extension.c_str());
	native_path_string	archive_part_path;
	archive_part_path.assignf_with_conversion	("%s/%s", directory.c_str(), out_archive_part_path->c_str());

	LOGI_INFO								("pack", m_args.log_format, m_args.log_flags, "saving archive part: '%s'", out_archive_part_path->c_str());

	saving_environment	archive_part_env	(* m_env.device);
	bool const create_result			=	create_archive_files(archive_part_path, archive_part_path, m_args.device, 
																 archive_part_env.db_file, archive_part_env.fat_file);
	R_ASSERT_U								(create_result);

	archive_part_env.save_root			=	node;
	archive_part_env.info_root			=	node_info;
	archive_part_env.node_index			=	m_env.node_index;
	archive_part_env.num_nodes			=	m_env.num_nodes;
	archive_part_env.archive_parts_count	=	m_env.archive_parts_count;
	archive_part_env.next_archive_part	=	m_env.next_archive_part + 1;
	archive_part_env.temp_name_index	=	m_env.temp_name_index;

	archive_saver							saver(m_args, m_hashset, archive_part_env);
	bool const result					=	saver.finished_successfully();
	R_ASSERT_U								(result);

	* out_archive_fat_size				=	saver.env().result_fat_size;

	m_env.next_archive_part				=	saver.env().next_archive_part;
	m_env.temp_name_index				=	saver.env().temp_name_index;
	m_env.node_index					=	saver.env().node_index;

	close_archive_files						(* m_env.device, archive_part_env.db_file, archive_part_env.fat_file);
}

void   archive_saver::save_sub_fat	    (native_path_string *	out_sub_fat_path,
										 u32 *					out_sub_fat_size,
										 base_node<> *			node, 
										 fat_node_info *		node_info)
{
	file_type *	sub_fat_file			=	NULL;

	get_path_without_last_item				(out_sub_fat_path, m_args.fat_path.c_str());
	out_sub_fat_path->appendf_with_conversion	("/~temp/temp_%d.fat", ++m_env.temp_name_index);
	LOGI_INFO								("pack", m_args.log_format, m_args.log_flags, "starting sub-fat: '%s'", node->get_name());

	create_folder_r							(m_args.device, * out_sub_fat_path, false);
	bool const open_result				=	m_args.device->open(& sub_fat_file, * out_sub_fat_path, 
																file_mode::create_always, file_access::write);
	R_ASSERT_U								(open_result);

	saving_environment	sub_fat_env			(* m_env.device);
	
	sub_fat_env.save_root				=	node;
	sub_fat_env.fat_file				=	sub_fat_file;
	sub_fat_env.db_file					=	m_env.db_file;
	sub_fat_env.info_root				=	node_info;
	sub_fat_env.node_index				=	m_env.node_index;
	sub_fat_env.num_nodes				=	m_env.num_nodes;
	sub_fat_env.next_archive_part		=	m_env.next_archive_part;
	sub_fat_env.temp_name_index			=	m_env.temp_name_index;

	archive_saver							saver(m_args, m_hashset, sub_fat_env);
	bool const result					=	saver.finished_successfully();
	R_ASSERT_U								(result);
	m_args.device->close					(sub_fat_file);

	* out_sub_fat_size					=	saver.env().result_fat_size;
	m_env.next_archive_part				=	saver.env().next_archive_part;
	m_env.temp_name_index				=	saver.env().temp_name_index;
	m_env.node_index					=	saver.env().node_index;

	LOGI_INFO								("pack", m_args.log_format, m_args.log_flags, "finished sub-fat: '%s'", node->get_name());
}

} // namespace vfs 
} // namespace xray 
