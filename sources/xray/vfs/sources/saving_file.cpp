////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "saving.h"
#include <xray/fs/device_utils.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/file_type_pointer.h>

namespace xray {
namespace vfs {

using namespace fs_new;

fat_node_info *   get_info_by_node		(base_node<> * node);

void   archive_saver::save_node_file	(fat_node_info * *					out_duplicate,
									  	 file_size_type *					out_pos,
									  	 bool *								out_is_compressed,
									  	 mutable_buffer *					out_inlined_data,
									  	 u32 *								out_compressed_size,
									  	 u32 *								out_file_hash,
									  	 base_node<> *						node, 
									  	 fat_node_info * 					node_info,
									  	 native_path_string const &			sub_fat_path,
									  	 u32								sub_fat_size)
{
	synchronous_device_interface & device	=	m_args.device;
	* out_pos							=	file_size_type(-1);
	* out_is_compressed					=	false;
	* out_compressed_size				=	u32(-1);
	* out_duplicate						=	NULL;

	bool const is_sub_fat				=	sub_fat_path.length() != 0;

	native_path_string	original_node_path;
	file_size_type original_file_offs	=	0;
	if ( is_sub_fat )
		original_node_path				=	sub_fat_path;
	else
	{
		original_node_path				=	get_node_physical_path(node);
		original_file_offs				=	get_file_offs(node);
	}

	file_size_type	sub_db_file_size	=	0;
	u32 file_size						=	u32(-1);
	if ( is_sub_fat )
	{
		bool const calculate_result		=	calculate_file_size	(device, & sub_db_file_size, sub_fat_path);
		R_ASSERT_U							(calculate_result);
	}
	else
	{
		file_size						=	get_raw_file_size(node);
		if ( m_args.flags & save_flag_forbid_empty_files )
			R_ASSERT						(file_size, "empty file packing forbidden by user. [%s]", node->m_name);
	}

	if ( !is_sub_fat )
	{
		virtual_path_string					node_path;
		node->get_full_path					(& node_path);

		fat_node_info * const duplicate	=	find_duplicate_file(node_info);

		if ( duplicate )
		{
			* out_duplicate				=	duplicate;
			if ( duplicate->is_compressed )
			{
				* out_is_compressed		=	true;
				* out_compressed_size	=	duplicate->compressed_size;
			}
			* out_file_hash				=	duplicate->hash;
			* out_pos					=	duplicate->pos;
			return;
		}
	}

	file_type_pointer f_in					(original_node_path, device, file_mode::open_existing, file_access::read);
	R_ASSERT								(f_in);
	device->seek							(f_in, original_file_offs, seek_file_begin);

	* out_pos							=	device->tell(m_env.db_file);

	if ( !is_sub_fat &&
		  m_args.compressor && 
		 (m_args.compress_smallest_rate > 0 || m_args.inline_data.highest_compression_rate() > 0) && 
		 !node->has_flags(vfs_node_is_compressed) )
	{
		R_ASSERT							(m_args.allocator);
		pstr src_data					=	(pstr)XRAY_ALLOC_IMPL(* m_args.allocator, char, file_size);
		u32 const dest_size				=	128 + (2 * file_size);
		pstr dest_data					=	(pstr)XRAY_ALLOC_IMPL(* m_args.allocator, char, dest_size);

		mutable_buffer const src_buffer	=	memory::buffer(src_data, file_size);
		device->read						(f_in, src_buffer.c_ptr(), file_size);

		mutable_buffer dest_buffer		=	memory::buffer(dest_data, dest_size);
		memory::zero						(dest_buffer);
		bool const res					=	m_args.compressor->compress(src_buffer, dest_buffer, * out_compressed_size);

		R_ASSERT							(res);
		if ( res )
		{
			bool allow_compression_in_db	=	true;
			fat_inline_data::item * inline_item	=	NULL;
			if ( m_args.inline_data.find_by_extension(& inline_item, extension_from_path(node->get_name())) )
				allow_compression_in_db	=	inline_item->allow_compression_in_db;

			float const rate			=	(float)* out_compressed_size / (file_size ? file_size : 1);
			if ( m_args.inline_data.try_fit_for_inlining(node->get_name(), file_size, * out_compressed_size) )
			{
				* out_inlined_data		=	memory::buffer(dest_data, * out_compressed_size);
				* out_file_hash			=	crc32((pcstr)dest_buffer.c_ptr(), * out_compressed_size);
				
				dest_data				=	NULL; // no deletion
				* out_is_compressed		=	true;
			}
			else if ( allow_compression_in_db && rate <= m_args.compress_smallest_rate )
			{
				device->write				(m_env.db_file, dest_buffer.c_ptr(), * out_compressed_size);
				* out_file_hash			=	crc32((pcstr)dest_buffer.c_ptr(), * out_is_compressed);
				* out_is_compressed		=	true;
			}
		}

		XRAY_FREE_IMPL						(* m_args.allocator, src_data);
		XRAY_FREE_IMPL						(* m_args.allocator, dest_data);

		if ( !* out_is_compressed )
			device->seek					(f_in, original_file_offs, seek_file_begin);
	}

	if ( !is_sub_fat &&
		 !* out_is_compressed && 
		 m_args.inline_data.try_fit_for_inlining(node->get_name(), file_size, 0) )
	{
		* out_inlined_data				=	memory::buffer((pstr)XRAY_ALLOC_IMPL(* m_args.allocator, char, file_size), file_size);
		mutable_buffer writer			=	* out_inlined_data;
		device->read						(f_in, writer.c_ptr(), file_size);
		
		* out_file_hash					=	crc32((pcstr)writer.c_ptr(), file_size);
	}
	else if ( !* out_is_compressed )
		copy_data							(device,
											 m_env.db_file, 
											 f_in, 
											 is_sub_fat ? sub_db_file_size : file_size, 
											 out_file_hash,
											 sub_fat_size ? sub_fat_size : file_size);

	f_in.close								();

	if ( !is_sub_fat )
	{
		node_info->node					=	node;
		node_info->pos					=	* out_pos;
		node_info->is_compressed		=	* out_is_compressed;

		if ( * out_is_compressed )
			node_info->compressed_size	=	* out_compressed_size;

		if ( !saving_patch() )
			m_env.node_info_set->insert		(node_info->hash, node_info);
	}
}

} // namespace vfs
} // namespace xray