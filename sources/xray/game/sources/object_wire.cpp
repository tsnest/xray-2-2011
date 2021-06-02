////////////////////////////////////////////////////////////////////////////
//	Created		: 31.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_wire.h"
#include "game_world.h"
#include "game.h"
#include <xray/render/engine/model_format.h>
#include <xray/render/facade/scene_renderer.h>

namespace stalker2{

object_wire::object_wire( game_world& w )
:super		( w ),
m_points	( NULL )
{}

object_wire::~object_wire( )
{
	FREE(m_points);
}

static void create_wire_visual_source( memory::writer& writer,
								float3* points,
								u32 points_count,
								pcstr material_name, 
								float wire_width )
{
	render::model_header	hdr;

	//// fill hdr here !!!
	hdr.platform_id			= render::platform_uni;
	hdr.bb.min				= float3(-1, -1, -1);
	hdr.bb.max				= float3(1, 1, 1);
	hdr.type				= render::mt_user_mesh_wire;

	writer.open_chunk		( render::model_chunk_header );
	writer.write			( &hdr, sizeof(hdr) );
	writer.close_chunk		( );//model_chunk_header

	writer.open_chunk		( render::model_texture );
    writer.write_string		( material_name );
	writer.write_float		( wire_width ); // wire width
	writer.close_chunk		( );//model_texture

	writer.open_chunk		( render::model_chunk_vertices );

	writer.write_u32		( points_count );

	for(u32 i=0; i<points_count; ++i)
		writer.write_float3(points[i]);

	writer.close_chunk		( );//model_chunk_vertices}
}


void object_wire::load( configs::binary_config_value const& t )
{
	super::load		( t );
	m_wire_width	= t["wire_width"];
	m_points_count	= t["points_count"];

	if(m_points_count!=0)
	{
		m_points		= ALLOC(float3, m_points_count);
		float3* p		= m_points;
		for(u32 i=0; i<m_points_count; ++i)
		{
			float3 pt	= t["points"][i];
			*p			= pt;
			++p;
		}
	}

}

void object_wire::load_contents( )
{
	if( m_visual )
		return;

	memory::writer	writer(g_allocator);
	writer.external_data	= true;

	pcstr material_name = "editor/wire";

	if(m_points_count && m_points)
	{
	create_wire_visual_source( writer, 
								m_points,
								m_points_count,
								material_name, 
								m_wire_width );


	const_buffer creation_buffer( writer.pointer(), writer.size() );
	fixed_string<32> wire_name;
	wire_name.assignf("wire_%X", this);
	resources::query_create_resource(
		wire_name.c_str(),
		creation_buffer,
		resources::user_mesh_class,
		boost::bind(&object_wire::resources_ready, this, _1),
		g_allocator
	);
	}else
	{
		LOG_WARNING("empty wire");
	}
}

void object_wire::unload_contents( )
{
	if( m_visual )
	{
		m_game_world.get_game().renderer().scene().remove_model( m_game_world.get_game().get_render_scene(), 
																m_visual );
		m_visual = NULL;
	}

}

void object_wire::resources_ready( resources::queries_result& data )
{
	R_ASSERT							(data.is_successful());
	const_buffer user_data_to_create	= data[0].creation_data_from_user();
	pbyte creation_data					= (pbyte)user_data_to_create.c_ptr();
	DELETE								( creation_data );

	m_visual							= static_cast_resource_ptr<render::render_model_instance_ptr>(data[0].get_unmanaged_resource());

	m_game_world.get_game().renderer().scene().add_model( m_game_world.get_game().get_render_scene(), 
															m_visual, m_transform );
}

} //namespace stalker2