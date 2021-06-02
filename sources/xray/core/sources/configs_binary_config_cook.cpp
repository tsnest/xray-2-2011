////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "configs_binary_config_cook.h"
#include "configs_binary_config.h"
#include <xray/resources_fs.h>
#include <xray/memory_stream.h>
#include <xray/resources_unmanaged_allocator.h>
#include <xray/resources_fs.h>

namespace xray {
namespace core {
namespace configs {

using namespace xray::resources;

binary_config_cook_impl::binary_config_cook_impl( ) 
:unmanaged_cook(config_class, reuse_true, use_resource_manager_thread_id, use_resource_manager_thread_id) 
{
}

mutable_buffer   binary_config_cook_impl::allocate_resource	(query_result_for_cook &	in_query, 
														 	 const_buffer				raw_file_data, 
														 	 bool						file_exist)
{
	XRAY_UNREFERENCED_PARAMETERS	( &in_query, &raw_file_data, file_exist );
	pvoid const buffer				= XRAY_MALLOC_IMPL( unmanaged_allocator(), sizeof(binary_config), "binary config");
	if ( !buffer ) {
		in_query.set_out_of_memory	( unmanaged_memory, sizeof(binary_config) );
		in_query.finish_query		( result_out_of_memory );
		return						mutable_buffer( 0, 0 );
	}

	return							mutable_buffer( buffer, sizeof(binary_config) );
}

void   binary_config_cook_impl::destroy_resource	(unmanaged_resource * resource)
{
	binary_config * config			= static_cast_checked<binary_config *>(resource);
	R_ASSERT						(config);
	config->~binary_config			();
}

void   binary_config_cook_impl::deallocate_resource	(pvoid buffer)
{
	XRAY_FREE_IMPL					(unmanaged_allocator(), buffer);
}

void   binary_config_cook_impl::create_resource (query_result_for_cook &	in_out_query, 
												 const_buffer				raw_file_data,
												 mutable_buffer				in_out_unmanaged_resource_buffer)
{
	binary_config * const config	= new (in_out_unmanaged_resource_buffer.c_ptr()) binary_config 
		((pcbyte)raw_file_data.c_ptr(), raw_file_data.size(), unmanaged_allocator());

	in_out_query.set_unmanaged_resource	(config, nocache_memory, in_out_unmanaged_resource_buffer.size());
	in_out_query.finish_query			(result_success);
}

static pcstr resources_converted_string	=	"resources.converted/";
static pcstr resources					=	"resources/";
static pcstr mounts_converted_string	=	"mounts.converted/";
static pcstr mounts_string				=	"mounts/";

void make_converted_path				( fs_new::virtual_path_string *			out_converted_path, 
										  fs_new::virtual_path_string const &	in_source_path )
{
	* out_converted_path				=	in_source_path;

	if ( in_source_path.find(resources) != u32(-1) )
	{
		out_converted_path->replace			(resources, resources_converted_string);
	}
	else if ( in_source_path.find(mounts_string) != u32(-1) )
	{
		out_converted_path->replace			(mounts_string, mounts_converted_string);
	}
	else 
	{
		*out_converted_path				=	resources_converted_string;
		*out_converted_path				+=	in_source_path;
	}
} 

binary_config_cook::binary_config_cook	( ) : 
	super	(
		binary_config_class, 
#ifdef MASTER_GOLD
		reuse_true, 
#else // #ifdef MASTER_GOLD
		reuse_false, 
#endif // #ifdef MASTER_GOLD
		use_resource_manager_thread_id
	)
{
}

void binary_config_cook::translate_query	( query_result_for_cook & parent )
{
	fs_new::virtual_path_string const		source_path	=	parent.get_requested_path();
	R_ASSERT								(source_path.length());
	fs_new::virtual_path_string				converted_path;
	make_converted_path						(& converted_path, source_path);

	request	fs_iterator_requests[]		=	{	{ source_path.c_str(), fs_iterator_class }, 
												{ converted_path.c_str(), fs_iterator_class } };

	query_resources				(
		fs_iterator_requests, 
		boost::bind(&binary_config_cook::on_fs_iterators_ready, this, _1), 
		helper_allocator(),
		0,
		& parent
	);
}

void binary_config_cook::on_fs_iterators_ready	(queries_result & results)
{
	query_result_for_cook * const parent	=	results.get_parent_query();
	fs_new::virtual_path_string const		source_path	=	parent->get_requested_path();
	fs_new::virtual_path_string				converted_path;
	make_converted_path						(& converted_path, source_path);

	vfs::vfs_iterator const & source_path_it	=	results[0].get_result_iterator();

#ifndef	MASTER_GOLD
	if ( !source_path_it ) 
	{
		parent->finish_query				(query_result_for_user::error_type_file_not_found, assert_on_fail_false);
		return;
	}
#endif // #ifndef	MASTER_GOLD

	vfs::vfs_iterator const & converted_path_it	=	results[1].get_result_iterator();

	fs_new::physical_path_info const source_info	=	get_physical_path_info(source_path_it);
	fs_new::physical_path_info const converted_info	=	get_physical_path_info(converted_path_it);

	bool const force_lua_mount_config	=	
#ifdef MASTER_GOLD
		false;
#else // #ifdef MASTER_GOLD
		(source_path.find(mounts_string) == 0);
#endif // #ifdef MASTER_GOLD


 	if	( 
			!identity(force_lua_mount_config) &&
			converted_info.exists() &&
 			(source_info.last_time_of_write() < converted_info.last_time_of_write()) 
		)
 	{
		// load converted binary config
 		query_resource(
 			converted_path.c_str(),
 			config_class,
 			boost::bind(&binary_config_cook::on_binary_config_loaded, this, _1, parent),
 			helper_allocator(),
			NULL,
			parent
 		);
 	}
 	else
 	{
#ifndef	MASTER_GOLD
	// load lua config
 		query_resource(
 			source_path.c_str(),
 			lua_config_class,
 			boost::bind(& binary_config_cook::on_lua_config_loaded, this, _1, parent),
 			helper_allocator(),
			NULL,
			parent
 			);
#else // #ifndef MASTER_GOLD
		FATAL( "cannot find binary config [%s]!", source_path.c_str() );
#endif // #ifndef MASTER_GOLD
 	}
}

#ifndef MASTER_GOLD
void   binary_config_cook::on_lua_config_loaded	(queries_result & data, query_result_for_cook * parent_query)
{
	if ( !data.is_successful() ) 
	{
		parent_query->finish_query			(data[0].get_error_type(), parent_query->assert_on_fail());
		return;
	}

	xray::configs::lua_config_ptr source_config_ptr		= static_cast_resource_ptr< xray::configs::lua_config_ptr >
															( data[0].get_unmanaged_resource() );

	memory::stream str						( &debug::g_mt_allocator );
	create_binary_config_buffer				( source_config_ptr->get_root(), str );
	mutable_buffer buffer					= mutable_buffer( str.get_buffer(), str.get_buffer_size() );

	fs_new::virtual_path_string				converted_path;
	make_converted_path						(& converted_path, parent_query->get_requested_path());
	fs_new::native_path_string				physical_path;
	bool const convertion_result		=	convert_virtual_to_physical_path(& physical_path, converted_path, NULL);
	R_ASSERT								(convertion_result);

	parent_query->save_generated_resource	(save_generated_data::create(buffer, true, physical_path, converted_path));

	xray::configs::binary_config_ptr resource_config_ptr	= xray::configs::create_binary_config( buffer );

	parent_query->set_unmanaged_resource	( resource_config_ptr.c_ptr(), nocache_memory, sizeof(binary_config) );
	parent_query->finish_query				( result_success );
}
#endif // #ifndef MASTER_GOLD

void binary_config_cook::on_binary_config_loaded( queries_result& data, query_result_for_cook* parent_query )
{
	R_ASSERT				( data.is_successful(), data[0].get_requested_path() );

	query_result_for_user const& result = data[0];

	xray::configs::binary_config_ptr resource_config_ptr = static_cast_resource_ptr<xray::configs::binary_config_ptr>(result.get_unmanaged_resource());

	parent_query->set_unmanaged_resource(resource_config_ptr.c_ptr(), nocache_memory, sizeof(binary_config));
	parent_query->finish_query(result_success);
}

void binary_config_cook::delete_resource( resource_base * resource )
{
	binary_config* config	= static_cast_checked<binary_config*>(resource);
	R_ASSERT				( config );
	memory::base_allocator * const allocator	=	config->get_allocator();
	XRAY_DELETE_IMPL		(allocator, config);	
}

} // namespace configs
} // namespace core
} // namespace xray
