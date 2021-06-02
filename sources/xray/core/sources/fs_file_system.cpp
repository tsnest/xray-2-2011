////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include "fs_file_system.h"
#include "fs_macros.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include <xray/fs_path_iterator.h>
#include "fs_helper.h"
#include "fs_watcher_internal.h"

namespace xray {
namespace fs   {

using namespace resources;

//-----------------------------------------------------------------------------------
// global file_system object
//-----------------------------------------------------------------------------------

uninitialized_reference<file_system>	g_fat;

void   set_allocator_thread_id (u32 thread_id)
{
	memory::g_fs_allocator.user_thread_id	(thread_id);
}

void   set_on_resource_leaked_callback	(on_resource_leaked_callback callback)
{
	g_fat->set_on_resource_leaked_callback	(callback);
}

void   set_on_unmount_started_callback	(on_unmount_started_callback callback)
{
	g_fat->set_on_unmount_started_callback	(callback);
}

//-----------------------------------------------------------------------------------
// file_system
//-----------------------------------------------------------------------------------

bool   fat_inline_data::find_by_extension (item const * * out_item, pcstr extension) const
{
	item * item	=	NULL;
	fat_inline_data * this_non_const	=	const_cast<fat_inline_data *>(this);
	if ( this_non_const->find_by_extension(& item, extension) )
	{
		* out_item						=	item;
		return								true;
	}

	return									false;
}

bool   fat_inline_data::find_by_extension (item * * out_item, pcstr extension)
{
	for ( container::iterator		it	=	m_items->begin(), 
								end_it	=	m_items->end();
									it	!=	end_it;
								  ++it	)
	{
		if ( it->extension == extension )
		{
			* out_item				=	& * it;
			return						true;			
		}
	}

	return								false;
}

u32   fat_inline_data::total_size_for_extensions_with_limited_size () const
{
	u32 total_size					=	0;
	for ( container::const_iterator	it	=	m_items->begin(), 
								end_it	=	m_items->end();
									it	!=	end_it;
								  ++it	)
	{
		if ( it->current_size != item::no_limit )
			total_size				+=	(it->current_size * 2) + (it->current_size < 10 ? 10 : 0);
	}

	return								total_size;
}

void   fat_inline_data::push_back (item const & item) 
{ 
	m_items->push_back					(item); 
	m_highest_compression_rate			=	math::max(m_highest_compression_rate, item.compression_rate); 
}

void   fat_inline_data::begin_fitting ()
{
	for ( container::iterator	it	=	m_items->begin(), 
								end_it	=	m_items->end();
								it	!=	end_it;
							  ++it	)
	{
		it->current_size			=	it->max_size;
	}
}

bool   fat_inline_data::try_fit_for_inlining (pcstr file_name, u32 const file_size, u32 const compressed_size)
{
	typedef item	item_type;
	item * item;
	if ( find_by_extension(& item, extension_from_path(file_name)) )
	{
		if ( compressed_size )
		{
			float const rate		=	(float)compressed_size / (file_size ? file_size : 1);
			if ( rate > item->compression_rate )
				return					false;
		}

		if ( item->current_size == item_type::no_limit )
			return						true;

		u32 const size				=	compressed_size ? compressed_size : file_size;
		if ( item->current_size >= size )
		{
			item->current_size			-=	size;
			return						true;
		}
	}

	return								false;
}

//-----------------------------------------------------------------------------------
// file_system
//-----------------------------------------------------------------------------------
file_system::file_system() : m_impl( FS_NEW(file_system_impl) )
{
}

file_system::~file_system()
{
	FS_DELETE							(m_impl);
}

void   file_system::mount_disk (pcstr fat_dir, pcstr disk_dir, watch_directory_bool const watch_directory, recursive_bool is_recursive)
{
	path_string	const relative_path	=	convert_to_relative_path(disk_dir);
	m_impl->mount_disk					(fat_dir, relative_path.c_str(), watch_directory, is_recursive);
}

fs_iterator   file_system::continue_mount_disk (pcstr fat_dir, recursive_bool recursive)
{
	fat_node<> * const node			=	m_impl->continue_mount_disk(NULL, fat_dir, recursive);
	fat_node<> * const link_target	=	find_referenced_link_node(node);
	return								fs_iterator(node, link_target, 
													recursive ? fs_iterator::type_recursive : fs_iterator::type_non_recursive);
}

bool   file_system::mount_db (pcstr logical_path, 
							  pcstr fat_file_path, 
							  pcstr db_file_path,
							  bool  need_replication)
{
	path_string	const relative_fat_path	=	convert_to_relative_path(fat_file_path);
	path_string	const relative_db_path	=	db_file_path ? convert_to_relative_path(db_file_path) : "";

	return m_impl->mount_db				(logical_path, 
										 relative_fat_path.c_str(), 
										 relative_db_path.length() ? 
												relative_db_path.c_str() : relative_fat_path.c_str(), 										 
										 need_replication);
}

void   file_system::unmount_disk (pcstr logical_path, pcstr disk_dir_path)
{
	path_string	const relative_path	=	convert_to_relative_path(disk_dir_path);
	m_impl->unmount_disk				(logical_path, relative_path.c_str());
}

void   file_system::unmount_db (pcstr logical_path, pcstr disk_path)
{
	path_string	const relative_path	=	convert_to_relative_path(disk_path);
	m_impl->unmount_db					(logical_path, relative_path.c_str());
}

bool   file_system::save_db (pcstr							fat_file_path, 
							 pcstr							db_file_path, 
							 bool							no_duplicates, 
							 u32							fat_alignment,
							 memory::base_allocator *		allocator,
							 compressor *					compressor,
							 float							compress_smallest_rate,
							 db_target_platform_enum		db_format,
							 fat_inline_data &				inline_data,
							 u32							fat_part_max_size,
							 db_callback					callback)
{
	CURE_ASSERT							(fat_alignment != 0, return false);
	struct cleanup
	{
		file_type *	db_file;
		file_type *	fat_file;

		cleanup() : db_file(NULL), fat_file(NULL) {}
	   ~cleanup()
		{
			if ( db_file )
				close_file				(db_file);
			if ( db_file != fat_file && fat_file )
				close_file				(fat_file);
		}
	} data;

	if ( db_file_path )
	{
		if ( !open_file(&data.db_file, open_file_write | open_file_create | open_file_truncate, db_file_path) )
			return						false;
	}

	bool fat_in_db					=	!fat_file_path ||
										(db_file_path && strings::equal(db_file_path, fat_file_path));
		
	if ( fat_in_db )
	{
		data.fat_file				=	 data.db_file;
	}	
	else
	{
		if ( !open_file(&data.fat_file, open_file_write | open_file_create | open_file_truncate, fat_file_path) )
			data.fat_file			=	NULL;
	}

	if ( !data.fat_file )
		return							false;
	
	saving_environment				env;

	env.fat_disk_path				=	fat_file_path;
	env.logical_path				=	"";
	env.callback					=	callback;
	env.fat_file					=	data.fat_file;
	env.db_file						=	data.db_file;
	env.fat_alignment				=	fat_alignment;
	env.fat_part_max_size			=	fat_part_max_size;
	env.no_duplicates				=	no_duplicates;
	env.db_format					=	db_format;
	env.alloc						=	allocator;
	env.compressor					=	compressor;
	env.compress_smallest_rate		=	compress_smallest_rate;
	env.inline_data					=	& inline_data;
	env.offset_for_file_positions	=	0;
	env.info_root					=	NULL;

	if ( !m_impl->save_db(env, NULL) )
		return							false;

	return								true;
}

bool   file_system::unpack (pcstr fat_dir, pcstr dest_dir, db_callback callback)
{
	return m_impl->unpack				(fat_dir, dest_dir, callback);
}

void   file_system::clear ()
{
	m_impl->clear						();
}

bool   file_system::try_find_sync (sub_fat_pin_fs_iterator *	out_pin_iterator,													 
								   pcstr const					path_to_find,
								   enum_flags<find_enum>		find_flags)
{
	return								m_impl->try_find_sync(out_pin_iterator, path_to_find, find_flags);
}

bool   file_system::try_find_sync_no_pin (fs_iterator *				out_iterator,
										  find_results_struct *		find_results,
										  pcstr const				path_to_find,
										  enum_flags<find_enum>		find_flags)
{
	return								m_impl->try_find_sync_no_pin	(out_iterator,
																		 find_results,
																		 path_to_find,
																		 find_flags);
}

void   file_system::find_async (find_results_struct *	find_results,
							    pcstr const				path_to_find,
								enum_flags<find_enum>	find_flags)
{
	m_impl->find_async					(find_results, path_to_find, find_flags);
}

void   file_system::set_replication_dir (pcstr replication_dir)
{
	m_impl->set_replication_dir			(replication_dir);
}

void   file_system::get_disk_path (fat_node<> * node, path_string & out_path) const
{
	ASSERT								(node);
	if ( node )
		m_impl->get_disk_path			(node, out_path);
	else
		out_path.clear					();
}

void   file_system::replicate_path (pcstr path2replicate, path_string& out_path) const
{
	m_impl->replicate_path				(path2replicate, out_path);
}

void   file_system::commit_replication (fat_node<> * node)
{
	m_impl->commit_replication			(node);
}

bool   file_system::operator == (file_system const & f) const
{
	return								* m_impl == * f.m_impl;
}

bool   file_system::replicate_file (fat_node<> * node, pcbyte data)
{
	return	m_impl->replicate_file		(node, data);
}

bool   file_system::equal_db (fat_node<> * left, fat_node<> * right) const
{
	CURE_ASSERT							(left && right, return false);
	return								m_impl->equal_db(left, right);
}

fat_node<> *   file_system::create_temp_disk_node (memory::base_allocator * const allocator, pcstr const disk_path)
{
	if ( !disk_path )
		return					NULL;
	
	file_size_type file_size		=	0;
	if ( !calculate_file_size(&file_size, disk_path) )
		return					NULL;

	pcstr const last_slash	=	strrchr(disk_path, '/');
	pcstr		name		=	last_slash ? last_slash + 1 : disk_path;

	fat_disk_file_node<> * const out_file_node	=	m_impl->create_disk_file_node
								(0, name, strings::length(name), disk_path, strings::length(disk_path), (u32)file_size, allocator, false);

	return						out_file_node->cast_node();
}

void   file_system::destroy_temp_disk_node (memory::base_allocator* const allocator, fat_node<> * node)
{
	if ( node )
		m_impl->destroy_temp_disk_node	(allocator, node->cast_disk_file());
}

bool   file_system::mount_disk_node_by_logical_path (pcstr const logical_path, fs_iterator * const out_iterator, bool assert_on_fail)
{
	return								m_impl->mount_disk_node(logical_path, NULL, out_iterator, assert_on_fail);
}

bool   file_system::convert_physical_to_logical_path (path_string * out_logical_path, pcstr const physical_path)
{
	return	m_impl->convert_physical_to_logical_path (out_logical_path, physical_path, false);
}

bool   file_system::mount_disk_node_by_physical_path (pcstr const physical_path, fs_iterator * const out_iterator)
{
	path_string		logical_path;
	if ( !m_impl->convert_physical_to_logical_path (& logical_path, physical_path, false) )
		return							false;

	return								m_impl->mount_disk_node(logical_path.c_str(), physical_path, out_iterator);
}

void   file_system::unmount_disk_node (pcstr physical_path)
{
	m_impl->unmount_disk_node			(physical_path);
}

void   file_system::mark_disk_node_as_erased (pcstr physical_path)
{
	m_impl->mark_disk_node_as_erased	(physical_path);
}

bool   file_system::update_file_size_in_fat	(pcstr physical_path, bool do_resource_deassociation)
{
	path_string		logical_path;
	if ( !m_impl->convert_physical_to_logical_path(& logical_path, physical_path, false) )
		return							false;

	return	m_impl->update_file_size_in_fat	(logical_path.c_str(), physical_path, do_resource_deassociation);
}

bool   file_system::rename_disk_node (pcstr old_physical_path, pcstr new_physical_path, bool do_resource_deassociation)
{
	path_string		logical_path;
	if ( !m_impl->convert_physical_to_logical_path(& logical_path, old_physical_path, false) )
		return							false;

	m_impl->rename_disk_node	(logical_path.c_str(), do_resource_deassociation, old_physical_path, new_physical_path);
	return								true;
}

signalling_bool   file_system::get_disk_path_to_store_file (pcstr const logical_path, buffer_string * const out_disk_path, mount_filter const &	filter)
{
	return	m_impl->get_disk_path_to_store_file(logical_path, out_disk_path, NULL, true, filter);
}

void   file_system::set_on_resource_leaked_callback (on_resource_leaked_callback callback)
{
	m_impl->set_on_resource_leaked_callback	(callback);
}

void   file_system::set_on_unmount_started_callback (on_unmount_started_callback callback)
{
	m_impl->set_on_unmount_started_callback	(callback);
}

// void   file_system_iterator::print (pcstr offs) const
// {
// 	printf								("%s%s", offs, get_name());
// 
// 	if ( is_disk() )
// 	{
// 		pcstr disk_path				=	NULL;
// 		if ( !is_folder() )
// 		{
// 			disk_path				=	m_cur->cast_disk_file()->m_disk_path;
// 			printf						("  (file: %s)\n", disk_path);
// 		}
// 		else
// 		{
// 			printf						("\r\n");
// 		}
// 	}
// 	else
// 	{
// 		printf							("\r\n");
// 	}
// 
// 	fixed_string<64> child_offs		=	offs;
// 	child_offs						+=	"  ";
// 
// 	file_system_iterator	cur_child			=	children_begin();
// 	file_system_iterator	end_child			=	children_end();
// 
// 	while ( cur_child != end_child )
// 	{
// 		cur_child.print					(child_offs.c_str());
// 		++cur_child;
// 	}
// }

//-----------------------------------------------------------------------------------
// file_system initialization
//-----------------------------------------------------------------------------------

} // namespace fs
} // namespace xray
