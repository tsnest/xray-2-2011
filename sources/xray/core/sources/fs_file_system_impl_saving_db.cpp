////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include <xray/fs_utils.h>
#include "fs_helper.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// saving db/file_system
//-----------------------------------------------------------------------------------

bool   file_system_impl::save_db (	FILE *						const	fat_file,
						  			FILE *						const 	db_file,
									pcstr						const	logical_path, 
									bool						const	no_duplicates,
									u32							const	fat_alignment,
									memory::base_allocator *	const	alloc,
									compressor *				const 	compressor,
									float						const	compress_smallest_rate,
									file_system::db_target_platform_enum	const	db_format,
									fat_inline_data &					inline_data,
									db_callback					const	callback	)
{
	threading::mutex_raii		raii	(m_mount_mutex);
	verify_path_is_portable				(logical_path);

	u32				hash;
	fat_node<> *	save_root		=	find_node(logical_path, & hash);
	if ( !save_root )
		return							false;

	bool const		fat_in_db		=	(db_file == fat_file);

	u32 const num_nodes				=	save_root->get_num_nodes();
	u32	const max_buffer_size		=	save_root->get_max_fat_size(inline_data);

	u32	const db_file_origin		=	ftell(db_file);

	if ( fat_in_db )
	{
		u32 const buffer_size_aligned	=	math::align_up (max_buffer_size + (u32)sizeof(db_header), fat_alignment);
		fseek							(db_file, buffer_size_aligned , SEEK_CUR);
	}

	pstr			dest_buffer		=	FS_ALLOC(char, max_buffer_size);
	
	detail::helper_hash_set helper_set;
	detail::helper_node *	helper_nodes	
									=	no_duplicates ? FS_ALLOC(detail::helper_node, num_nodes) : NULL;

	// writing nodes and filling file_system
	save_nodes_environment env;

	env.cur_offs					=	0;
	env.callback					=	callback;
	env.db_file						=	db_file;
	env.dest_buffer					=	dest_buffer;
	env.dest_buffer_size			=	max_buffer_size;
	env.helper_nodes				=	helper_nodes;
	env.helper_set					=	& helper_set;
	env.node_index					=	0;
	env.db_format					=	db_format;
	env.num_nodes					=	num_nodes;
	env.alloc						=	alloc;
	env.compressor					=	compressor;
	env.compress_smallest_rate		=	compress_smallest_rate;
	env.inline_data					=	& inline_data;

	if ( env.db_format == file_system::db_target_platform_pc )
		save_nodes<pointer_for_fat_64bit>	(env, save_root, 0, 0);
	else
		save_nodes<pointer_for_fat_32bit>	(env, save_root, 0, 0);

	if ( fat_in_db )
		fseek							(db_file, db_file_origin, SEEK_SET);

	// writing file_system

	file_size_type const real_buffer_size	=	env.cur_offs;
	db_header	header;
	header.num_nodes				=	num_nodes;
	header.buffer_size				=	(u32)real_buffer_size;
	
	if ( file_system::is_big_endian_format(db_format) )
	{
		header.reverse_bytes			();
		header.set_big_endian			();
	}
	else
		header.set_little_endian		();
	
	fwrite								(& header, sizeof(db_header), 1, fat_file);
	fwrite								(dest_buffer, (size_t)real_buffer_size, 1, fat_file);

	FS_FREE								(dest_buffer);

	env.helper_set->clear				();
	if ( helper_nodes )
		FS_FREE							(helper_nodes);

	return								true;
}

using namespace detail;

void   file_system_impl::save_node_file (save_nodes_environment &	env, 
										  fat_node<> *				original_node, 
										  file_size_type &			out_pos,
										  bool &					is_compressed,
										  mutable_buffer *			out_inlined_data,
										  u32 &						compressed_size,
										  u32 &						out_file_hash)
{
	out_pos									=	u32(-1);
	is_compressed							=	false;
	compressed_size							=	u32(-1);

	path_string		original_node_path;
	get_disk_path								(original_node, original_node_path);
	FILE * f_in								=	NULL;
	bool const result						=	open_file(&f_in, open_file_read, original_node_path.c_str());
	XRAY_UNREFERENCED_PARAMETER					(result);
	R_ASSERT									(result);

	u32	const file_size						=	original_node->get_raw_file_size();

	if ( env.helper_nodes )
	{
		helper_hash_set::iterator it		=	env.helper_set->find(file_size);
		helper_hash_set::iterator end_it	=	env.helper_set->end();

		helper_node *	duplicate_node		=	it != end_it ? *it : NULL;
		bool			duplicated			=	false;

		while ( duplicate_node )
		{
			if ( duplicate_node->original_node->get_raw_file_size() == file_size )
			{
				path_string		path;
				get_disk_path					(duplicate_node->original_node, path);
	
				FILE *	   f_cur			=	NULL;
				bool const result			=	open_file(&f_cur, open_file_read, path.c_str());
				XRAY_UNREFERENCED_PARAMETER		(result);
				R_ASSERT						(result);
				duplicated					=	files_equal(f_cur, f_in, file_size);
				fclose							(f_cur);
				if ( duplicated )
					break;
			}
			
			duplicate_node					=	duplicate_node->get_next_hash_node();
		}

		if ( duplicated ) // found duplicate!
		{
			if ( duplicate_node->is_compressed )
			{
				is_compressed				=	true;
				compressed_size				=	duplicate_node->compressed_size;
			}
			out_pos							=	duplicate_node->pos;
			fclose								(f_in);
			return;
		}

		fseek									(f_in, 0, SEEK_SET);
	}

	out_pos									=	ftell(env.db_file);

	if ( env.compressor && 
		 (env.compress_smallest_rate > 0 || env.inline_data->highest_compression_rate() > 0) && 
		 !original_node->has_flags(file_system::is_compressed) )
	{
		R_ASSERT								(env.alloc);
		pstr src_data						=	(pstr)XRAY_ALLOC_IMPL(*env.alloc, char, file_size);
		u32 const dest_size					=	128 + (2 * file_size);
		pstr dest_data						=	(pstr)XRAY_ALLOC_IMPL(*env.alloc, char, dest_size);

		mutable_buffer const src_buffer		=	memory::buffer(src_data, file_size);
		fread									(src_buffer.c_ptr(), 1, file_size, f_in);

		mutable_buffer dest_buffer			=	memory::buffer(dest_data, dest_size);
		memory::zero							(dest_buffer);
		bool const res						=	env.compressor->compress(src_buffer, dest_buffer, compressed_size);

		R_ASSERT								(res);
		if ( res )
		{
			bool allow_compression_in_db	=	true;
			fat_inline_data::item * inline_item	=	NULL;
			if ( env.inline_data->find_by_extension(& inline_item, extension_from_path(original_node->get_name())) )
				allow_compression_in_db		=	inline_item->allow_compression_in_db;

			float const rate				=	(float)compressed_size / (file_size ? file_size : 1);
			if ( env.inline_data->try_fit_for_inlining(original_node->get_name(), file_size, compressed_size) )
			{
				* out_inlined_data			=	memory::buffer(dest_data, compressed_size);
				out_file_hash				=	crc32((pcstr)dest_buffer.c_ptr(), compressed_size);
				
				dest_data					=	NULL; // no deletion
				is_compressed				=	true;
			}
			else if ( allow_compression_in_db && rate <= env.compress_smallest_rate )
			{
				fwrite							(dest_buffer.c_ptr(), 1, compressed_size, env.db_file);
				out_file_hash				=	crc32((pcstr)dest_buffer.c_ptr(), compressed_size);
				is_compressed				=	true;
			}
		}

		XRAY_FREE_IMPL							(* env.alloc, src_data);
		XRAY_FREE_IMPL							(* env.alloc, dest_data);

		if ( !is_compressed )
			fseek								(f_in, 0, SEEK_SET);
	}

	if ( !is_compressed && 
		 env.inline_data->try_fit_for_inlining(original_node->get_name(), file_size, 0) )
	{
		* out_inlined_data					=	memory::buffer((pstr)XRAY_ALLOC_IMPL(* env.alloc, char, file_size), file_size);
		mutable_buffer writer				=	* out_inlined_data;
		fread									(writer.c_ptr(), file_size, 1, f_in);
		
		out_file_hash						=	crc32((pcstr)writer.c_ptr(), file_size);
	}
	else if ( !is_compressed )
		copy_data								(env.db_file, f_in, file_size, & out_file_hash);

	fclose										(f_in);

	if ( env.helper_nodes )
	{
		detail::helper_node * new_helper	=	env.helper_nodes;
		new_helper->helper_node::helper_node	();
		new_helper->original_node			=	original_node;
		new_helper->pos						=	out_pos;
		new_helper->is_compressed			=	is_compressed;

		if ( is_compressed )
			new_helper->compressed_size		=	compressed_size;

		env.helper_set->insert					(file_size, new_helper);
		++env.helper_nodes;
	}
}

template <pointer_for_fat_size_enum target_pointer_size>
struct save_nodes_helper
{
	template <class T>
	static u32 get_node_offs ()
	{
		u32 const offset			=	u32((pbyte)& ((T *)NULL)->m_base - (pbyte)((T *)NULL));
		return							offset;

	}

	static u32 get_node_offs (fat_node<target_pointer_size> * const node, bool do_reverse_bytes)
	{
		u16 node_flags				=	(u16)node->get_flags();
		COMPILE_ASSERT					(sizeof(node->m_flags) == sizeof(node_flags), change_node_flags_to_have_correct_size);
		if ( do_reverse_bytes )
			reverse_bytes				(node_flags);

		if ( node_flags & file_system::is_folder )
		{
			return						get_node_offs<fat_folder_node<target_pointer_size> >();
		}
		else if ( node_flags & file_system::is_inlined )
		{
			return						(node_flags & file_system::is_compressed) ?	
										get_node_offs<fat_db_inline_compressed_file_node<target_pointer_size> >() :
										get_node_offs<fat_db_inline_file_node<target_pointer_size> >();
		}
		else
		{
			return						(node_flags & file_system::is_compressed) ?	
										get_node_offs<fat_db_compressed_file_node<target_pointer_size> >() :
										get_node_offs<fat_db_file_node<target_pointer_size> >();
		}
	}
}; // helper

template <pointer_for_fat_size_enum target_pointer_size>
fat_node<target_pointer_size> *   file_system_impl::save_nodes  (save_nodes_environment &	env, 
							 									 fat_node<> * const			cur, 
							 									 file_size_type const		prev_offs, 
							 									 file_size_type const		parent_offs)
{
	bool const		do_reverse_bytes	=	file_system::is_big_endian_format(env.db_format);

	u32 const		name_len_with_zero	=	strings::length(cur->m_name) + 1;

	u32 const		max_node_class_size	=	math::max<u32>(	sizeof(fat_folder_node<target_pointer_size>), 
														sizeof(fat_db_compressed_file_node<target_pointer_size>),
														sizeof(fat_db_file_node<target_pointer_size>)	);

	u32				max_node_size	=	 max_node_class_size + name_len_with_zero + 16;

	char*				node_data	=	(char*)ALLOCA(max_node_size);
	memory::zero						(node_data, max_node_size);
	
	fat_folder_node<target_pointer_size> * const					temp_folder					=	(fat_folder_node<target_pointer_size> *)					node_data;
	fat_db_file_node<target_pointer_size> * const					temp_file					=	(fat_db_file_node<target_pointer_size> *)					node_data;
	fat_db_compressed_file_node<target_pointer_size> * const		temp_compressed_file		=	(fat_db_compressed_file_node<target_pointer_size> *)		node_data;
	fat_db_inline_file_node<target_pointer_size> * const			temp_inline_file			=	(fat_db_inline_file_node<target_pointer_size> *)			node_data;
	fat_db_inline_compressed_file_node<target_pointer_size> * const	temp_inline_compressed_file	=	(fat_db_inline_compressed_file_node<target_pointer_size> *)	node_data;

	fat_node<target_pointer_size> *	temp	=	NULL;
	bool	is_compressed			=	false;
	mutable_buffer	inlined_data;
	bool			is_inlined		=	false;

	u32		file_hash				=	0;
	u32		node_size				=	0;
	if ( cur->is_folder() )
	{
		new ( temp_folder )				fat_folder_node<target_pointer_size>;
		temp						=	temp_folder->cast_node();
		node_size					=	sizeof(fat_folder_node<target_pointer_size>);
	}
	else
	{
		file_size_type					pos_in_db;
		u32								compressed_size;
		save_node_file					(env, cur, pos_in_db, is_compressed, & inlined_data, compressed_size, file_hash);
		is_inlined					=	!!inlined_data.size();

		if ( is_inlined )
		{
			if ( is_compressed )
			{
				new ( temp_inline_compressed_file )						fat_db_inline_compressed_file_node<target_pointer_size>;
				temp_inline_compressed_file->m_uncompressed_size	=	cur->get_raw_file_size();
				temp_inline_compressed_file->m_size					=	compressed_size;
				temp_inline_compressed_file->m_pos					=	(typename fat_db_inline_compressed_file_node<target_pointer_size>::file_size_type)pos_in_db;
				temp												=	temp_inline_compressed_file->cast_node();
				node_size											=	sizeof(fat_db_inline_compressed_file_node<target_pointer_size>);
			}
			else
			{
				new ( temp_inline_file )		fat_db_inline_file_node<target_pointer_size>;
				temp_inline_file->m_size	=	cur->get_raw_file_size();
				temp_inline_file->m_pos		=	(typename fat_db_inline_file_node<target_pointer_size>::file_size_type)pos_in_db;
				temp						=	temp_inline_file->cast_node();
				node_size					=	sizeof(fat_db_inline_file_node<target_pointer_size>);
			}
		}
		else
		{
			if ( is_compressed )
			{
				new ( temp_compressed_file ) fat_db_compressed_file_node<target_pointer_size>;
				temp_compressed_file->m_uncompressed_size	=	cur->get_raw_file_size();
				temp_compressed_file->m_size				=	compressed_size;
				temp_compressed_file->m_pos					=	(typename fat_db_compressed_file_node<target_pointer_size>::file_size_type)pos_in_db;
				temp										=	temp_compressed_file->cast_node();
				node_size									=	sizeof(fat_db_compressed_file_node<target_pointer_size>);
			}
			else
			{
				new ( temp_file )			fat_db_file_node<target_pointer_size>;
				temp_file->m_size		=	cur->get_raw_file_size();
				temp_file->m_pos		=	(typename fat_db_file_node<target_pointer_size>::file_size_type)pos_in_db;
				temp					=	temp_file->cast_node();
				node_size				=	sizeof(fat_db_file_node<target_pointer_size>);
			}
		}
	}

	u32 node_alignment				=	8;
	if ( env.db_format != file_system::db_target_platform_pc )
	{
		if ( !is_inlined )
			node_alignment			=	4;
	}

	node_size						+=	name_len_with_zero;
	u32 padded_size					=	math::align_up(node_size, node_alignment);
	u32 const padded_size_with_inlined	=	is_inlined ? math::align_up(padded_size + inlined_data.size(), node_alignment) : padded_size;

	temp->m_flags					=	cur->m_flags;
	strings::copy						(temp->m_name, name_len_with_zero, cur->m_name);

	temp->m_flags					&=	~file_system::is_disk;
	temp->m_flags					|=	file_system::is_db;
	if ( is_compressed )
		temp->m_flags				|=	file_system::is_compressed;
	if ( is_inlined )
		temp->m_flags				|=	file_system::is_inlined;

	if ( !cur->is_folder() )
		temp->set_hash					(file_hash);

	if ( is_inlined )
		temp->set_inline_data			(memory::buffer((pbyte)(env.cur_offs + padded_size), inlined_data.size()));

	temp->set_next_hash_node			(NULL);

	pvoid const current_node_dest_pos	=	env.dest_buffer + env.cur_offs;
	file_size_type const current_node_dest_size	=	env.dest_buffer_size - env.cur_offs;
	R_ASSERT_CMP						(current_node_dest_size, >=, padded_size_with_inlined);

	temp->m_prev					=	cur->m_prev		? (fat_node<target_pointer_size> *)(size_t) prev_offs	:	NULL;
	temp->m_parent					=	cur->m_parent	? (fat_node<target_pointer_size> *)(size_t) parent_offs	:	NULL;

	file_size_type const new_parent_offs	=	env.cur_offs + save_nodes_helper<target_pointer_size>::get_node_offs(temp, false);
	env.cur_offs					+=	padded_size_with_inlined;
		
	fat_node<> *	child			=	cur->get_first_child();
	file_size_type	first_child_offs	=	u32(-1);
	file_size_type	child_prev_offs		=	NULL;
	fat_node<target_pointer_size> *	prev_fat_child	=	NULL;
	while ( child )
	{
		file_size_type	const saved_cur_offs	=	env.cur_offs;
		fat_node<target_pointer_size> * const saved_fat_child	=	save_nodes<target_pointer_size>(env, child, child_prev_offs, new_parent_offs);

		child_prev_offs				=	saved_cur_offs + save_nodes_helper<target_pointer_size>::get_node_offs(saved_fat_child, do_reverse_bytes);
		if ( first_child_offs == u32(-1) )
			first_child_offs		=	child_prev_offs;

		if ( prev_fat_child )
		{
			prev_fat_child->m_next	=	(fat_node<target_pointer_size> *)((pbyte)saved_fat_child - (pbyte)env.dest_buffer);

			if ( do_reverse_bytes )
				reverse_bytes			(prev_fat_child->m_next);
		}

		child						=	child->get_next();

		if ( !child )
			saved_fat_child->m_next	=	NULL;

		prev_fat_child				=	saved_fat_child;
	}

	if ( cur->is_folder() )
		temp_folder->m_first_child	=	(fat_node<target_pointer_size> *)(size_t)first_child_offs;

	u32 const non_reversed_flags	=	temp->get_flags(); // must be before reverse_bytes
	if ( do_reverse_bytes )
		temp->reverse_bytes_for_real_class	();

	memory::copy						(current_node_dest_pos, (size_t)current_node_dest_size, node_data, padded_size);
	if ( is_inlined )
	{
		pstr const inline_pos		=	(pstr)current_node_dest_pos + padded_size;
		size_t const buffer_left	=	(size_t)(current_node_dest_size - padded_size);
		memory::copy					(inline_pos, buffer_left, inlined_data.c_ptr(), inlined_data.size());
		size_t const pad_size_for_inlined	=	padded_size_with_inlined - padded_size - inlined_data.size();
		memory::fill32					(inline_pos + inlined_data.size(), pad_size_for_inlined, 0, pad_size_for_inlined);
		pvoid data					=	inlined_data.c_ptr();
		XRAY_FREE_IMPL					(env.alloc, data);
	}

	++env.node_index;
	if ( env.callback )
		env.callback					(env.node_index, env.num_nodes, temp->get_name(), non_reversed_flags);

	return								(fat_node<target_pointer_size> *)
										((pbyte)current_node_dest_pos + save_nodes_helper<target_pointer_size>::get_node_offs(temp, do_reverse_bytes));
}

} // namespace fs 
} // namespace xray 
