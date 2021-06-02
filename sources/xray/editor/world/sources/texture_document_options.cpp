////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_editor_source.h"


xray::render::texture_options::ETType get_texture_type_from_name( pcstr name )
{
	xray::fs_new::virtual_path_string	resource_path = name;
	xray::fs_new::virtual_path_string	resource_name;

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

