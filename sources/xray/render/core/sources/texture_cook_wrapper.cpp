////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_cook_wrapper.h"
#include "texture_converter_cook.h"
#include <xray/fs/device_utils.h>
#include <xray/resources_fs.h>

namespace xray {
namespace render {

using namespace resources;
using namespace fs_new;

// some constants to minimize magic
static u32 const source_index			=	0;
static u32 const options_index			=	1;
static u32 const converted_index		=	2;

static pcstr converted_texture_extension			=	"dds";
static pcstr source_texture_extension				=	"tga";
static pcstr source_texture_options_extension		=	"options";

static pcstr resources_textures_converted_string	=	"resources.converted/textures/";
static pcstr resources_textures_string				=	"resources/textures/";

texture_cook_wrapper::texture_cook_wrapper ()
	: translate_query_cook(
		texture_wrapper_class, 
#ifndef MASTER_GOLD
		reuse_false, 
#else /// #ifndef MASTER_GOLD
		reuse_true, 
#endif /// #ifndef MASTER_GOLD
		use_any_thread_id
	)
{
}

void   texture_cook_wrapper::make_converted_path (virtual_path_string *	out_converted_path, 
												  query_result_for_cook & result)
{
	out_converted_path->assign_replace		(result.get_requested_path(), 
											 resources_textures_string, 
											 resources_textures_converted_string);
	set_extension_for_path					(out_converted_path, converted_texture_extension);
}

void   texture_cook_wrapper::translate_query (query_result_for_cook & query)
{
#ifndef MASTER_GOLD
	virtual_path_string						converted_texture_path;
	make_converted_path						(& converted_texture_path, query);

	virtual_path_string	source_texture_path	= query.get_requested_path();
	set_extension_for_path					(& source_texture_path, source_texture_extension);

	virtual_path_string	options_texture_path	= query.get_requested_path();
	set_extension_for_path					(& options_texture_path, source_texture_options_extension);

	request	requests[3];
	requests[source_index].set				(source_texture_path.c_str(), fs_iterator_class);
	requests[options_index].set				(options_texture_path.c_str(), fs_iterator_class);
	requests[converted_index].set			(converted_texture_path.c_str(), fs_iterator_class);

	query_resources	(
		requests, 
		boost::bind( & texture_cook_wrapper::on_fs_iterators_ready, this, _1),
		& debug::g_mt_allocator,
		0,
		& query
	);

#else // #ifndef MASTER_GOLD

	query_converted_texture					(& query);

#endif // #ifndef MASTER_GOLD
}

void   texture_cook_wrapper::query_converted_texture (query_result_for_cook * parent)
{
	virtual_path_string						converted_texture_path;
	make_converted_path						(& converted_texture_path, * parent);

	query_resource							(converted_texture_path.c_str(),
											 texture_class,
											 boost::bind(&texture_cook_wrapper::on_texture_loaded, this, _1), 
#ifdef MASTER_GOLD
											 & memory::g_mt_allocator,
#else
											 & debug::g_mt_allocator,
#endif
											 NULL,
											 parent); 
}

#ifndef MASTER_GOLD

void   texture_cook_wrapper::on_fs_iterators_ready (queries_result & result)
{
	query_result_for_cook * const parent	=	result.get_parent_query();

	vfs::vfs_iterator const source_it		=	result[source_index].get_result_iterator();
	vfs::vfs_iterator const options_it		=	result[options_index].get_result_iterator();
	vfs::vfs_iterator const converted_it	=	result[converted_index].get_result_iterator();

	physical_path_info const & source_info		=	get_physical_path_info(source_it);
	physical_path_info const & options_info		=	get_physical_path_info(options_it);
	physical_path_info const & converted_info	=	get_physical_path_info(converted_it);

	if ( !source_info.exists() )
	{
		parent->finish_query				(result_error);
		return;
	}

	bool converted_got_old				=	!converted_info.exists();
	converted_got_old					=	converted_got_old ||
											(source_info.last_time_of_write() > converted_info.last_time_of_write()) || 
											(options_info.last_time_of_write() > converted_info.last_time_of_write());

	if ( converted_got_old )
	{
		request_convertion					(result[source_index].get_requested_path(), 
											 result[options_index].get_requested_path(), 
											 parent);
		return;
	}

	query_converted_texture					(parent);
}

void   texture_cook_wrapper::request_convertion (pcstr source_texture_path, 
												 pcstr options_texture_path, 
												 query_result_for_cook * parent)
{
	request	const requests[] =			{	{ options_texture_path,	texture_options_binary_class	},
											{ source_texture_path,	raw_data_class		},	};

	query_resources							(requests, array_size(requests), 
											 boost::bind(&texture_cook_wrapper::on_raw_data_loaded, this, _1), 
											 & debug::g_mt_allocator, 
											 NULL, 
											 parent); 
}


void   texture_cook_wrapper::on_raw_data_loaded (queries_result & result)
{
	query_result_for_cook * const parent	=	result.get_parent_query();

	if ( !result[1].is_successful())
	{
		parent->finish_query				(result[1].get_error_type(), result.assert_on_fail());
		return;
	}

	virtual_path_string						converted_texture_path;
	make_converted_path						(& converted_texture_path, * parent);

	texture_converter_params* const data_for_creation	=	DEBUG_NEW(texture_converter_params)(
		result[0].is_successful() ? result[0].get_unmanaged_resource() : 0,
		result[1].get_managed_resource(),
		converted_texture_path.c_str(),
		parent->get_requested_path()
	); 	

	query_create_resource
		("",
		 const_buffer ((pcvoid)data_for_creation, sizeof(texture_converter_params)),
		 texture_converter_class, 
		 boost::bind(& texture_cook_wrapper::on_texture_converted, this, _1, data_for_creation),
		 & debug::g_mt_allocator,
		 NULL,
		 parent);
}

void   texture_cook_wrapper::on_texture_converted (queries_result &	result, texture_converter_params * data_to_delete)
{
	DEBUG_DELETE							( data_to_delete );

	query_result_for_cook * const parent	=	result.get_parent_query();

	if ( !result.is_successful() )
	{
		parent->finish_query				(result[0].get_error_type(), parent->assert_on_fail());
		return;
	}

	managed_resource_ptr const converted_resource	=	result[0].get_managed_resource();

	pcbyte const pinned_data			=	converted_resource->pin();
	u32 const pinned_size				=	converted_resource->get_size();

	const_buffer const pinned_buffer		(pinned_data, pinned_size);

	virtual_path_string						converted_texture_path;
	make_converted_path						(& converted_texture_path, * parent);

	query_create_resource					(converted_texture_path.c_str(),
											 pinned_buffer,
											 texture_class,
											 boost::bind(& texture_cook_wrapper::on_texture_created, this, converted_resource, pinned_data, _1), 
											 & debug::g_mt_allocator,
											 0,
											 parent);
}

void   texture_cook_wrapper::on_texture_created (managed_resource_ptr converted_resource, pcbyte pinned_data, queries_result & result)
{
	R_ASSERT								(converted_resource && pinned_data);
	converted_resource->unpin				(pinned_data);	
	on_texture_loaded						(result);
}
#endif //#ifndef MASTER_GOLD

void   texture_cook_wrapper::on_texture_loaded (queries_result & result)
{
	query_result_for_cook * const	parent	=	result.get_parent_query();

	if ( !result.is_successful() )
	{
		parent->finish_query			(result_error);
		return;
	}

	parent->set_managed_resource	(result[0].get_managed_resource());
	parent->finish_query			(result_success);
}

void   texture_cook_wrapper::delete_resource (resource_base * )
{
	CURE_ASSERT						(identity(false), "should not end up here! Call Lain");
}

} // namespace render
} // namespace xray
