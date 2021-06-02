////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_helper.h"
#include "fs_impl_saving.h"

namespace xray {
namespace fs {

void   save_db_helper::save_node_file (saving_environment &	env, 
								       fat_node_info * *	out_duplicate,
									   file_size_type *		out_pos,
									   bool *				out_is_compressed,
									   mutable_buffer *		out_inlined_data,
									   u32 *				out_compressed_size,
									   u32 *				out_file_hash,
									   fat_node<> *			node, 
									   fat_node_info * 		node_info,
									   pcstr				sub_fat_path,
									   u32					sub_fat_size)
{
	* out_pos								=	u32(-1);
	* out_is_compressed						=	false;
	* out_compressed_size					=	u32(-1);
	* out_duplicate							=	NULL;

	bool const is_sub_fat					=	strings::length(sub_fat_path) != 0;

	path_string									original_node_path;
	if ( is_sub_fat )
		original_node_path					=	sub_fat_path;
	else
		env.fs->get_disk_path					(node, original_node_path);

	file_type * f_in						=	NULL;
	bool const result						=	open_file(&f_in, open_file_read, original_node_path.c_str());
	XRAY_UNREFERENCED_PARAMETER					(result);
	R_ASSERT									(result);

	file_size_type	sub_db_file_size		=	0;
	u32 file_size							=	u32(-1);
	if ( is_sub_fat )
	{
		bool const calculate_result			=	calculate_file_size	(& sub_db_file_size, sub_fat_path);
		R_ASSERT_U								(calculate_result);
	}
	else
		file_size							=	node->get_raw_file_size();

	if ( !is_sub_fat )
	{
		fat_node_info *	duplicate			=	NULL;
		for ( fat_node_info_set::iterator	it		=	env.node_info_set->find(file_size),
											end_it	=	env.node_info_set->end();
											it		!=	end_it;
											++it )
		{
			fat_node_info *	const it_node	=	* it;

			if ( it_node->node->is_folder() )
				continue;

			if ( it_node->node->get_raw_file_size() != file_size )
				continue;

			path_string							path;
			env.fs->get_disk_path				(it_node->node, path);

			file_type *	   f_cur			=	NULL;
			bool const success				=	open_file(&f_cur, open_file_read, path.c_str());
			R_ASSERT_U							(success);
			bool const equal_files			=	files_equal(f_cur, f_in, file_size);
			close_file							(f_cur);
			if ( equal_files )
			{
				duplicate					=	it_node;
				break;
			}
		}

//#pragma message(XRAY_TODO("turned off links"))
		if ( duplicate ) //&& xray::identity(0) )
		{
			* out_duplicate					=	duplicate;
			if ( duplicate->is_compressed )
			{
				* out_is_compressed			=	true;
				* out_compressed_size		=	duplicate->compressed_size;
			}
			* out_file_hash					=	duplicate->hash;
			* out_pos						=	duplicate->pos;
			close_file							(f_in);
			return;
		}

		seek_file								(f_in, 0, SEEK_SET);
	}

	* out_pos								=	env.offset_for_file_positions + tell_file(env.db_file);

	if ( !is_sub_fat &&
		  env.compressor && 
		 (env.compress_smallest_rate > 0 || env.inline_data->highest_compression_rate() > 0) && 
		 !node->has_flags(file_system::is_compressed) )
	{
		R_ASSERT								(env.alloc);
		pstr src_data						=	(pstr)XRAY_ALLOC_IMPL(*env.alloc, char, file_size);
		u32 const dest_size					=	128 + (2 * file_size);
		pstr dest_data						=	(pstr)XRAY_ALLOC_IMPL(*env.alloc, char, dest_size);

		mutable_buffer const src_buffer		=	memory::buffer(src_data, file_size);
		read_file								(f_in, src_buffer.c_ptr(), file_size);

		mutable_buffer dest_buffer			=	memory::buffer(dest_data, dest_size);
		memory::zero							(dest_buffer);
		bool const res						=	env.compressor->compress(src_buffer, dest_buffer, * out_compressed_size);

		R_ASSERT								(res);
		if ( res )
		{
			bool allow_compression_in_db	=	true;
			fat_inline_data::item * inline_item	=	NULL;
			if ( env.inline_data->find_by_extension(& inline_item, extension_from_path(node->get_name())) )
				allow_compression_in_db		=	inline_item->allow_compression_in_db;

			float const rate				=	(float)* out_compressed_size / (file_size ? file_size : 1);
			if ( env.inline_data->try_fit_for_inlining(node->get_name(), file_size, * out_compressed_size) )
			{
				* out_inlined_data			=	memory::buffer(dest_data, * out_compressed_size);
				* out_file_hash				=	crc32((pcstr)dest_buffer.c_ptr(), * out_compressed_size);
				
				dest_data					=	NULL; // no deletion
				* out_is_compressed			=	true;
			}
			else if ( allow_compression_in_db && rate <= env.compress_smallest_rate )
			{
				write_file						(env.db_file, dest_buffer.c_ptr(), * out_compressed_size);
				* out_file_hash				=	crc32((pcstr)dest_buffer.c_ptr(), * out_is_compressed);
				* out_is_compressed			=	true;
			}
		}

		XRAY_FREE_IMPL							(* env.alloc, src_data);
		XRAY_FREE_IMPL							(* env.alloc, dest_data);

		if ( !* out_is_compressed )
			seek_file							(f_in, 0, SEEK_SET);
	}

	if ( !is_sub_fat &&
		 !* out_is_compressed && 
		 env.inline_data->try_fit_for_inlining(node->get_name(), file_size, 0) )
	{
		* out_inlined_data					=	memory::buffer((pstr)XRAY_ALLOC_IMPL(* env.alloc, char, file_size), file_size);
		mutable_buffer writer				=	* out_inlined_data;
		read_file								(f_in, writer.c_ptr(), file_size);
		
		* out_file_hash						=	crc32((pcstr)writer.c_ptr(), file_size);
	}
	else if ( !* out_is_compressed )
		copy_data								(env.db_file, 
												 f_in, 
												 is_sub_fat ? sub_db_file_size : file_size, 
												 out_file_hash,
												 sub_fat_size ? sub_fat_size : file_size);

	close_file									(f_in);

	if ( !is_sub_fat )
	{
		node_info->node						=	node;
		node_info->pos						=	* out_pos;
		node_info->is_compressed			=	* out_is_compressed;

		if ( * out_is_compressed )
			node_info->compressed_size		=	* out_compressed_size;

		env.node_info_set->insert				(file_size, node_info);
	}
}

} // namespace fs
} // namespace xray