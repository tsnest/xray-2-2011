////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/pack_archive.h>
#include <xray/vfs/mount_args.h>
#include <xray/vfs/virtual_file_system.h>
#include <xray/vfs/unpack.h>

namespace xray {
namespace vfs {

using namespace		resources;
using namespace		fs_new;

void   db_callback						(u32								num_nodes, 
										 u32								whole_nodes, 
										 pcstr								name, 
										 u32								flags, 
										 logging::log_format *				log_format,
										 logging::log_flags_enum			log_flags,										 	
										 debug_info_enum					debug_info)
{
	if ( debug_info == debug_info_detail )
	{
		fixed_string512						flags_string;
		if ( flags & vfs_node_is_inlined )
			flags_string				+=	"inlined+";
		if ( flags & vfs_node_is_compressed )
			flags_string				+=	"compressed+";

		if ( flags_string.length() )
			flags_string.set_length	(flags_string.length() - 1);

		LOGI_INFO							("pack", log_format, log_flags,  
											 "%d of %d files proceeded: %s %s", num_nodes, whole_nodes, name, flags_string.c_str());
	}
	else if ( debug_info == debug_info_normal )
	{
		if ( num_nodes == whole_nodes || !(num_nodes%25) )
		{
			LOGI_INFO						("pack",  log_format, log_flags,  
											 "%d of %d files proceeded", num_nodes, whole_nodes);
		}
	}
}

static void   mount_callback			(mount_result result)
{
	R_ASSERT								(result.result == result_success);
}

bool   mount_sources					(virtual_file_system &				vfs,
										 synchronous_device_interface & 	synchronous_device, 
										 sources_array &					sources, 
										 mount_ptrs_array &					mount_ptrs,
										 memory::base_allocator *			allocator,
										 logging::log_format *				log_format,
										 logging::log_flags_enum			log_flags)
{
	for ( sources_array::iterator it	=	sources.begin();
								  it	!=	sources.end();
								++it )
	{
		native_path_string const & path	=	* it;

		physical_path_info const path_info	=	synchronous_device->get_physical_path_info(path);

		query_mount_arguments	mount_args;
		
		if ( path_info.is_folder() )
		{
			LOGI_INFO						("pack", "mounting physical path: '%s'", path.c_str());
			mount_args					=	query_mount_arguments::mount_physical_path
												(allocator, 
												"",
												path,
												"",
												NULL,
												& synchronous_device,
												& mount_callback, 
												recursive_true);
		}
		else if ( path_info.is_file() )
		{
			LOGI_INFO						("pack", "mounting archive: '%s'", path.c_str());
			mount_args					=	query_mount_arguments::mount_archive
												(allocator, 
												"",
												path,
												path,
												"",
												NULL,
												& synchronous_device,
												& mount_callback,
												lock_operation_lock);
		}
		else
		{
			LOGI_ERROR						("pack", log_format, log_flags, "can't find anything to mount on path: '%s'", path.c_str());
			return							false;
		}

		vfs_mount_ptr	mount_ptr		=	query_mount_and_wait(vfs, mount_args).mount;
		if ( !mount_ptr )
			return							false;

		mount_ptrs.push_back				(mount_ptr);
	}

	return									true;
}

struct paths_predicate_type
{
	paths_predicate_type				(sources_array * sources) : sources(sources) {}

	bool operator ()					(int , pcstr string, u32 , bool) const
	{
		sources->push_back					(string);
		return								true;
	}

	sources_array *	sources;
};

void   parse_sources					(native_path_string const & sources_string, sources_array * const out_sources)
{
	paths_predicate_type paths_predicate	(out_sources);
	strings::iterate_items					(sources_string.c_str(), sources_string.length(), paths_predicate, ';');
}

} // namespace vfs
} // namespace xray