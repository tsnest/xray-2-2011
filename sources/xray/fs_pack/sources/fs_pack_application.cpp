#include "pch.h"
#include "fs_pack_application.h"
#include "fs_pack_memory.h"

#include <xray/buffer_string.h>
#include <xray/engine/api.h>
#include <xray/compressor_ppmd.h>

#include <stdio.h>

#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMSG
#undef NOWINMESSAGES
#undef NOCTLMGR
#include <xray/os_include.h>

#include <xray/core/core.h>

#include <xray/core/sources/fs_file_system.h>
#include <xray/fs_path.h>
#include <xray/core/sources/fs_macros.h>
#include <xray/resources_fs.h>
#include <xray/core/simple_engine.h>
#include <xray/resources.h>
#include <xray/configs_lua_config.h>

using namespace xray;

namespace fs_pack {

using namespace							xray::fs;
using namespace							xray::resources;

xray::memory::doug_lea_allocator_type	g_fs_pack_allocator;

static 	xray::core::simple_engine		core_engine;

enum   debug_info_enum { debug_info_disabled, debug_info_normal, debug_info_detail };
static debug_info_enum s_debug_info	=	debug_info_disabled;

void   db_callback (u32 num_nodes, u32 whole_nodes, pcstr name, u32 flags)
{
	if ( s_debug_info == debug_info_detail )
	{
		fixed_string512 flags_string;
		if ( flags & file_system::is_inlined )
			flags_string	+=	"inlined+";
		if ( flags & file_system::is_compressed )
			flags_string	+=	"compressed+";

		if ( flags_string.length() )
			flags_string.set_length	(flags_string.length() - 1);

		printf("%d of %d files proceeded: %s %s\n", num_nodes, whole_nodes, name, flags_string.c_str());
	}
	else if ( s_debug_info == debug_info_normal )
	{
		if ( num_nodes == whole_nodes || !(num_nodes%25) )
			printf("%d of %d files proceeded\n", num_nodes, whole_nodes);
	}
}

void   application::initialize (u32 argc, pstr const* argv)
{
	m_argv								=	argv;
	m_argc								=	argc;

	xray::core::preinitialize				(&core_engine,
											 core::create_log, 
											 GetCommandLine(), 
											 command_line::contains_application_true,
											 "fs_pack",
											 __DATE__);

	g_fs_pack_allocator.do_register			(256*1024*1024,	"fs_pack");
	g_fs_pack_allocator.user_current_thread_id();

	xray::memory::allocate_region			( );

	xray::core::initialize					( "../../resources/sources/", "fs_pack", core::perform_debug_initialization );
}

void   application::finalize ()
{
	xray::core::finalize				( );
}

command_line::key	debug_info_command				("debug_info", "", "fs_pack", "outputs debug info", "normal|detail");
command_line::key	fat_command						("fat", "", "fs_pack", "fat-dest file");
command_line::key	db_command						("db", "", "fs_pack", "db-dest file");
command_line::key	no_duplicates_command			("no_duplicates", "", "fs_pack", "prevent storing duplicate files (slower packing)");
command_line::key	unpack_command					("unpack", "", "fs_pack", "unpack", "unpack dest dir");
command_line::key	pack_command					("pack", "", "fs_pack", "pack");
command_line::key	target_platform					("target_platform", "", "fs_pack", "fat file compatible with specific platform", "pc|xbox360|ps3");
command_line::key	align_fat_command				("align_fat", "", "fs_pack", "", "fat-size alignment");
command_line::key	archive_rate_command			("archive_rate", "", "fs_pack", "", "archiving rate which is small enough to archive");
command_line::key	fat_part_max_size_command		("fat_part_max_size", "", "fs_pack", "", "subdivision of fat into parts criteria");
command_line::key	sources_command					("sources", "src", "fs_pack", "", "paths to sources separated with ';'");

command_line::key	inline_config_command			("inline_config", "", "fs_pack", "", "path to inline config");


static		fixed_vector<fat_inline_data::item, 1024>	s_inline_container;
static		fat_inline_data								s_inline_data(& s_inline_container);
		
void   on_inline_config_loaded (resources::queries_result & in_result)
{
	path_string		inline_config_path;
	inline_config_command.is_set_as_string	(& inline_config_path);
	R_ASSERT					(in_result.is_successful(), "cannot find inline config specified: %s" );

	configs::lua_config_ptr config_ptr = static_cast_checked<configs::lua_config *>(in_result[0].get_unmanaged_resource().c_ptr());
	configs::lua_config_value const & config	=	config_ptr->get_root();

	for ( configs::lua_config::const_iterator	it	=	config.begin();
												it	!=	config.end();
												++it )
	{
		configs::lua_config_value const & value	=	* it;
		fat_inline_data::item	item;
		item.extension				=	it.key();
		if ( !value.value_exists("total_size") )
			R_ASSERT					("total_size is not set for value: '%s'", value.get_field_id());
		
		if ( value.value_exists("allow_compression_in_db") )
		{
			configs::lua_config_value const & allow_compression_in_db	=	value["allow_compression_in_db"];
			if ( allow_compression_in_db.get_type() == configs::t_integer )
				item.allow_compression_in_db	=	!!(u32)allow_compression_in_db;
			else
				FATAL					("allow_compression_in_db for value '%s' has wrong type, must be integer", value.get_field_id());
		}

		configs::lua_config_value const & total_size_value	=	value["total_size"];
		if ( total_size_value.get_type() == configs::t_string )
		{
			if ( strings::equal((pcstr)total_size_value, "no_limit") )
			{
				item.current_size = item.max_size =	(u32)fat_inline_data::item::no_limit;
			}
			else
				FATAL					("wrong total_size '%s' for value '%s'", value.get_field_id(), (pcstr)total_size_value);
		}
		else if ( total_size_value.get_type() == configs::t_integer )
		{
			item.current_size = item.max_size =	(u32)total_size_value;
		}
		else FATAL						("total_size for value '%s' has wrong type, must be integer or 'no_limit'", value.get_field_id());
				
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

		s_inline_data.push_back				(item);
	}
}

void   application::execute ()
{
	m_exit_code							=	0;
#ifndef XRAY_STATIC_LIBRARIES
	command_line::check_keys				();
#endif // #ifndef XRAY_STATIC_LIBRARIES

	typedef	vector<fs::path_string>			sources_array;
	sources_array		sources;

	fixed_string4096	paths;
	sources_command.is_set_as_string		(& paths);
	struct paths_predicate_type
	{
		paths_predicate_type(sources_array * sources) : sources(sources) {}

		bool operator () (int , pcstr string, u32 , bool) const
		{
			sources->push_back				(string);
			return							true;
		}

		sources_array *	sources;
	} paths_predicate(& sources);

	file_system::db_target_platform_enum	platform	=	file_system::db_target_platform_unset;
	fixed_string512	platform_as_string;
	target_platform.is_set_as_string		(& platform_as_string);

	if ( platform_as_string.find("pc") != platform_as_string.npos )
		platform						=	file_system::db_target_platform_pc;
	else if ( platform_as_string.find("xbox360") != platform_as_string.npos )
		platform						=	file_system::db_target_platform_xbox360;
	else if ( platform_as_string.find("ps3") != platform_as_string.npos )
		platform						=	file_system::db_target_platform_ps3;
	
	strings::iterate_items					(paths.c_str(), paths.length(), paths_predicate, ';');
	if ( platform == file_system::db_target_platform_unset || !sources.size() || (!pack_command && !unpack_command) || (pack_command && unpack_command) )
	{
		if ( platform == file_system::db_target_platform_unset )
			printf							("you must set 'target_platform' key to one of supported platforms : pc, xbox360, ps3\n");

		if ( !sources.size() )
			printf							("you must set 'sources' key\n");

		if ( (!pack_command && !unpack_command) )
			printf							("you must set 'pack' or 'unpack' key\n");

		if ( pack_command && unpack_command )
			printf							("you must not use both 'pack' and 'unpack' key\n");

		m_exit_code						=	1;
		command_line::show_help_and_exit	();
		return;
	}

	fs::path_string			fat_file;
	fs::path_string			db_file;
	fs::path_string			unpack_dir;
	sub_fat_resource_ptr	sub_fat;

	fat_command.is_set_as_string			(& fat_file);
	db_command.is_set_as_string				(& db_file);
	unpack_command.is_set_as_string			(& unpack_dir);

	u32 fat_part_max_size			=	u32(-1);
	fat_part_max_size_command.is_set_as_number	(& fat_part_max_size);

	float			rate				=	0;
	archive_rate_command.is_set_as_number	(& rate);

	u32				fat_align			=	2048;
	align_fat_command.is_set_as_number		(& fat_align);
		
	set_allocator_thread_id					(threading::current_thread_id());
	file_system		fat;

	if ( pack_command )
	{
		path_string	config_file;
		if ( inline_config_command.is_set_as_string(& config_file) )
		{
			path_string	query_path		=	"@";
			query_path					+=	config_file;

			xray::resources::query_resource_and_wait	(convert_to_portable(query_path.c_str()).c_str(), xray::resources::lua_config_class, & on_inline_config_loaded, & g_fs_pack_allocator);
			xray::resources::wait_and_dispatch_callbacks(true);
		}

		//-----------------------------------------------------------------------------------
		// packing
		//-----------------------------------------------------------------------------------

		if ( !db_file.length() )
			db_file						=	"fs_pack_default_output.db";

		if ( !fat_file.length() )
			fat_file					=	db_file;

		for ( sources_array::iterator it	=	sources.begin();
									  it	!=	sources.end();
									++it )
		{
			pcstr			path		=	(*it).c_str();
			path_string		portable	=	convert_to_portable(path);

			printf							("mounting disk: %s ...\n", portable.c_str());
			fat.mount_disk					("", portable.c_str(), 
											 file_system::watch_directory_false, 
											 file_system::is_recursive_true);
		}

		fs_iterator							root_it;
		find_results_struct					find_results(find_results_struct::type_no_pin);
		bool const found_root			=	fat.try_find_sync_no_pin(& root_it, & find_results, "", find_recursively);
		R_ASSERT							(found_root);
 		printf								("mounted	nodes: %d\n", root_it.get_num_nodes());
		printf								("writing db...\n" );

		compressor * compressor			=	NULL;
		if ( archive_rate_command )
		{
			ppmd_compressor* const ppmd	=	XRAY_NEW_IMPL(g_fs_pack_allocator, ppmd_compressor) 
											(&g_fs_pack_allocator, 32);
			compressor					=	ppmd;
		}

		fixed_string512		debug_info;
		if ( debug_info_command.is_set_as_string(& debug_info) )
		{
			if ( debug_info == "detail" )
				s_debug_info			=	debug_info_detail;
			else if ( debug_info == "normal" )
				s_debug_info			=	debug_info_normal;
			else 
			{
				printf						("%s key can be set to either 'normal' or 'detail'\n", debug_info_command.full_name());
				m_exit_code				=	1;
				command_line::show_help_and_exit();
				return;
			}
		}

		if ( fat.save_db(fat_file.c_str(), 
						 db_file.c_str(), 
						 no_duplicates_command, 
						 fat_align, 
						 & g_fs_pack_allocator, 
						 compressor, 
						 rate, 
						 platform, 
						 s_inline_data,
						 fat_part_max_size,
						 s_debug_info == debug_info_disabled ? NULL : db_callback) )
		{
			if ( db_file == fat_file )
				printf						("written db with fat into: %s\n", db_file);
			else
			{
				printf						("written db  to: %s\n", db_file);
				printf						("written fat to: %s\n", fat_file);
			}
		}
		else
			printf							("an error was returned\n");

		XRAY_DELETE_IMPL					(g_fs_pack_allocator, compressor);
	}
	else
	{
		//-----------------------------------------------------------------------------------
		// unpacking
		//-----------------------------------------------------------------------------------
		for (	sources_array::iterator it	=	sources.begin();
										it	!=	sources.end();
									  ++it )
		{
			pcstr			path		=	(*it).c_str();
			path_string		portable	=	convert_to_portable(path);
		
			
			printf							("mounting db: %s ... ", portable.c_str());
  			if ( !fat.mount_db("", portable.c_str(), portable.c_str(), false) )
				printf						("failed\n");
			else
				printf						("successfull\n");
		}

		printf								("unpacking to %s... ", unpack_dir);
		if ( !fat.unpack("", unpack_dir.c_str(), db_callback) )
			printf							("failed\n");
		else
			printf							("successfull\n");
	}

	resources::wait_and_dispatch_callbacks	(true);

// 	file_system in_fat;
// 	in_fat.mount_db						("", fat_file, NULL, false, false);
// 
// 	file_system::iterator	real_it		=	in_fat.find("");
// 	
// 	resources::fs_iterator	it;
// 	*(void**)&it						=	*(void**)&real_it;
// 	log_fs_iterator							(it);
	
	m_exit_code							=	core_engine.get_exit_code();
}

} // namespace fs_pack
