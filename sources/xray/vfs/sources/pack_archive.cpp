////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/pack_archive.h>
#include <xray/resources.h>
#include <xray/vfs/virtual_file_system.h>
#include <xray/fs/device_utils.h>

namespace xray {
namespace vfs {

using namespace		resources;
using namespace		fs_new;

#ifndef MASTER_GOLD

void   on_inline_config_loaded			(queries_result & in_result, fat_inline_data * inline_data)
{
	R_ASSERT									(in_result.is_successful(), "cannot find inline config specified: %s" );

	configs::lua_config_ptr config_ptr = static_cast_checked<configs::lua_config *>(in_result[0].get_unmanaged_resource().c_ptr());
	configs::lua_config_value const & config	=	config_ptr->get_root();

	for ( configs::lua_config::const_iterator	it	=	config.begin();
												it	!=	config.end();
												++it )
	{
		configs::lua_config_value const & value	=	* it;
		fat_inline_data::item	item;
		item.extension					=	(pcstr)it.key();
		if ( !value.value_exists("total_size") )
			R_ASSERT						("total_size is not set for value: '%s'", value.get_field_id());
		
		if ( value.value_exists("allow_compression_in_db") )
		{
			configs::lua_config_value const & allow_compression_in_db	=	value["allow_compression_in_db"];
			if ( allow_compression_in_db.get_type() == configs::t_integer )
				item.allow_compression_in_db	=	!!(u32)allow_compression_in_db;
			else
				FATAL						("allow_compression_in_db for value '%s' has wrong type, must be integer", value.get_field_id());
		}

		configs::lua_config_value const & total_size_value	=	value["total_size"];
		if ( total_size_value.get_type() == configs::t_string )
		{
			if ( strings::equal((pcstr)total_size_value, "no_limit") )
			{
				item.current_size = item.max_size =	(u32)fat_inline_data::item::no_limit;
			}
			else
				FATAL						("wrong total_size '%s' for value '%s'", value.get_field_id(), (pcstr)total_size_value);
		}
		else if ( total_size_value.get_type() == configs::t_integer )
		{
			item.current_size = item.max_size =	(u32)total_size_value;
		}
		else FATAL							("total_size for value '%s' has wrong type, must be integer or 'no_limit'", value.get_field_id());
				
		if ( value.value_exists("compression_rate") )
		{
			configs::lua_config_value const & compression_rate_value	=	value["compression_rate"];

			if ( compression_rate_value.get_type() == configs::t_float )
				item.compression_rate	=	(float)compression_rate_value;
			else if ( compression_rate_value.get_type() == configs::t_integer )
				item.compression_rate	=	(float)(u32)compression_rate_value;
			else 
				FATAL						("compression_rate for value '%s' must be float type", value.get_field_id());
		}

		inline_data->push_back				(item);
	}
}

#endif // #ifndef MASTER_GOLD

bool   pack_archive						(pack_archive_args & args)
{
	fixed_vector<fat_inline_data::item, 1024>	inline_container;
	fat_inline_data								inline_data(& inline_container);

#ifndef MASTER_GOLD
	if ( args.config_file.length() )
	{
		fixed_string512	query_path		=	"@";
		query_path						+=	args.config_file.c_str();

		portable_path_string const	portable_path	=	portable_path_string::convert(query_path.c_str());

		query_resource_and_wait				(portable_path.c_str(), 
											 lua_config_class, 
											 boost::bind(& on_inline_config_loaded, _1, & inline_data), 
											 args.allocator);

		wait_and_dispatch_callbacks			(true);
	}
#endif // #ifndef MASTER_GOLD


	u32 const sources_size				=	args.sources.count_of(';') + 1;
	sources_array							sources(ALLOCA(sizeof(native_path_string) * sources_size), sources_size);
	parse_sources							(args.sources, & sources);

	mount_ptrs_array	mount_ptrs			((vfs_mount_ptr *)ALLOCA(sizeof(vfs_mount_ptr) * sources.size()), sources.size());
	virtual_file_system local_vfs;
	virtual_file_system & vfs			=	args.vfs ? * args.vfs : local_vfs;
	if ( !args.vfs )
		mount_sources						(vfs, args.synchronous_device, sources, mount_ptrs, args.allocator, args.log_format, args.log_flags);

	vfs_locked_iterator	vfs_iterator;
	result_enum const find_result		=	vfs.find_async("", & vfs_iterator, find_recursively, args.allocator, NULL);
	XRAY_UNREFERENCED_PARAMETER				(find_result);
	R_ASSERT								(find_result == result_success);
	R_ASSERT								(vfs_iterator);

	LOGI_INFO								("pack", args.log_format, args.log_flags, "mounted nodes: %d", vfs_iterator.get_nodes_count());
	LOGI_INFO								("pack", args.log_format, args.log_flags, "writing db..");

	compressor * compressor				=	NULL;
	if ( args.compression_rate > 0 )
	{
		ppmd_compressor* const ppmd		=	XRAY_NEW_IMPL(args.allocator, ppmd_compressor) 
											(args.allocator, 32);
		compressor						=	ppmd;
	}

	save_archive_callback callback		=	NULL;
	if ( args.debug_info != debug_info_disabled )
		callback						=	boost::bind(& db_callback, _1, _2, _3, _4, args.log_format, args.log_flags, args.debug_info);

	save_archive_args	save_args			(args.synchronous_device,
											 native_path_string(args.target_fat),
											 native_path_string(args.target_db),
											 args.flags,
											 args.fat_align,
											 args.allocator,
											 compressor,
											 args.compression_rate,
											 args.platform,
											 inline_data,
									 		 args.fat_part_max_size,
											 args.archive_part_max_size,
											 callback,
											 args.log_format, 
											 args.log_flags,
											 args.from_vfs,
											 args.from_vfs_info_tree,
											 args.to_vfs,
											 args.to_vfs_info_tree);

	if ( vfs.save_archive(save_args) )
	{
		if ( args.target_db == args.target_fat )
			LOGI_INFO						("pack", args.log_format, args.log_flags, "written db with fat into: %s\n", args.target_db.c_str());
		else
		{
			LOGI_INFO						("pack", args.log_format, args.log_flags, "written db  to: %s\n", args.target_db.c_str());
			LOGI_INFO						("pack", args.log_format, args.log_flags, "written fat to: %s\n", args.target_fat.c_str());
		}
	}
	else
		LOGI_ERROR							("pack", args.log_format, args.log_flags, "error occured");

	if ( args.clean_temp_files )
	{
		fs_new::native_path_string			temp_path;
		get_path_without_last_item			(& temp_path, args.target_fat.c_str());
		temp_path.append_path				("~temp");
		fs_new::erase_r						(args.synchronous_device, temp_path);
	}

	vfs_iterator.clear						();
	mount_ptrs.clear						();

	XRAY_DELETE_IMPL						(args.allocator, compressor);
	return									true;
}

} // namespace vfs
} // namespace xray
