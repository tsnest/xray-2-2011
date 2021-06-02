////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_cook.h"
#include <xray/render/core/texture_options.h>

namespace xray{
namespace render{

xray::render::texture_options::ETType get_texture_type_from_name( pcstr name )
{
	xray::fs_new::virtual_path_string resource_path = name;
	xray::fs_new::virtual_path_string resource_name;

	int finded_position = resource_path.rfind('/');
	resource_name.assign(resource_path.begin()+finded_position+1, resource_path.end());

	if(resource_name.find("terrain") == 0)
	{
		u32 offset = resource_name.rfind('_');
		if( offset != -1 )
		{
			if( resource_name.find( "bump", offset ) != -1 )	return xray::render::texture_options::terrain_bump;
			if( resource_name.find( "color", offset ) != -1 )	return xray::render::texture_options::terrain_color;
		}
		return xray::render::texture_options::terrain;
	}else
	{
		u32 offset = resource_name.rfind('_');
		if( offset != -1 )
		{
			if( resource_name.find( "sky", offset ) != -1 ) 			return xray::render::texture_options::sky;
			if( resource_name.find( "bump", offset ) != -1 )			return xray::render::texture_options::bump;
			if( resource_name.find( "nmap", offset ) != -1 )			return xray::render::texture_options::normal_map;
		}
		return xray::render::texture_options::type_2d;
	}
}

// texture options cookers
texture_options_cooker::texture_options_cooker	( resources::class_id_enum resource_class, reuse_enum reuse_type, u32 translate_query_thread, enum_flags<flags_enum> flags )
:super(resource_class, reuse_type, translate_query_thread, flags )
{}

void texture_options_cooker::translate_request_path(pcstr request_path, fs_new::virtual_path_string& new_request) const
{
	if( 0==strstr(request_path, ".options") )
		new_request.assignf	("resources/textures/%s.options", request_path);
	else
		new_request = request_path;
}

void texture_options_cooker::delete_resource( resources::resource_base* )
{
}

#ifndef MASTER_GOLD
texture_options_lua_cooker::texture_options_lua_cooker( ) 
:super( resources::texture_options_lua_class, reuse_true, use_resource_manager_thread_id) 
{
}


void texture_options_lua_cooker::translate_query( resources::query_result_for_cook& parent )
{
	pcstr path = parent.get_requested_path();

	resources::query_resource(
					path, 
					resources::lua_config_class, 
					boost::bind(&texture_options_lua_cooker::on_lua_config_loaded, this, _1), 
					resources::unmanaged_allocator(), 
					NULL,
					&parent); 

}

void texture_options_lua_cooker::on_lua_config_loaded( resources::queries_result& result )
{
	resources::query_result_for_cook* parent	= result.get_parent_query();

	if( result[0].is_successful())
	{
		// result[0] == lua config
		resources::unmanaged_resource_ptr	resource =	result[0].get_unmanaged_resource( );

		parent->set_unmanaged_resource	( resource, resource->memory_usage( ) );
		parent->finish_query			( result_success);
	}else
	{
		create_default_options( parent );
	}
}

void texture_options_lua_cooker::create_default_options( resources::query_result_for_cook* parent )
{
	fs_new::virtual_path_string resource_name = parent->get_requested_path();

	configs::lua_config_ptr config_ptr	= configs::create_lua_config( resource_name.c_str() );
	configs::lua_config_value options	= config_ptr->get_root()["options"];

	options["ver"]						= "1.2";
	options["texture_dimension"]		= xray::render::texture_options::texture_2D;
	options["has_mip_levels"]			= 1;
	options["bump_mode"]				= xray::render::texture_options::paralax;
	
	xray::render::texture_options::ETType t = get_texture_type_from_name(resource_name.c_str());

	options["srgb"]						= s32(t!=xray::render::texture_options::normal_map);
	options["high_quality"]				= 0;
	
	if(t==xray::render::texture_options::normal_map)
		options["format"]					= "NORMAL_MAP";
	else	
		options["format"]					= "DXT1";
	
	config_ptr->save				( configs::target_sources );
	
	parent->set_unmanaged_resource	( config_ptr.c_ptr(), resources::nocache_memory, sizeof(configs::lua_config));
	parent->finish_query			( result_success );
}
#endif // #ifndef MASTER_GOLD

texture_options_binary_cooker::texture_options_binary_cooker( ) 
:super( resources::texture_options_binary_class, reuse_true, use_resource_manager_thread_id) 
{
}

void texture_options_binary_cooker::translate_query( resources::query_result_for_cook& parent )
{
	pcstr path = parent.get_requested_path();

	resources::query_resource(
					path, 
					resources::binary_config_class, 
					boost::bind(&texture_options_binary_cooker::on_binary_config_loaded, this, _1), 
					resources::unmanaged_allocator(), 
					NULL,
					&parent); 
}

void texture_options_binary_cooker::on_binary_config_loaded( resources::queries_result& result )
{
	resources::query_result_for_cook* parent	= result.get_parent_query();

	if( result[0].is_successful())
	{
		// result[0] == binary config
		resources::unmanaged_resource_ptr	resource =	result[0].get_unmanaged_resource( );

		parent->set_unmanaged_resource	( resource, resource->memory_usage( ) );
		parent->finish_query			( result_success);
	}else
	{
		pcstr path = parent->get_requested_path();
		resources::query_resource(
						path, 
						resources::texture_options_lua_class, 
						boost::bind(&texture_options_binary_cooker::on_lua_options_loaded, this, _1), 
						resources::unmanaged_allocator(), 
						NULL,
						parent); 
		
	}
}

void texture_options_binary_cooker::on_lua_options_loaded( resources::queries_result& result )
{
	resources::query_result_for_cook* parent	= result.get_parent_query();

	R_ASSERT						( result[0].is_successful() );
	parent->finish_query			( result_requery );
}

} //namespace render
} //namespace xray