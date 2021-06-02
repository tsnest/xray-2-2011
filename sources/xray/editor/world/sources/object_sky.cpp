////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_sky.h"
#include "object_base.h"
#include "project.h"

#pragma managed(push, off)
#	include <xray/render/facade/debug_renderer.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/facade/decal_properties.h>
#	include <xray/render/facade/material_effects_instance_cook_data.h>
#	include <xray/render/facade/vertex_input_type.h>
#pragma managed(pop)

namespace xray {
namespace editor {

//////////////////////////////////////////			I N I T I A L I Z E				 //////////////////////////////////////////////

object_sky::object_sky( tool_base^ t, render::scene_ptr const& scene ):
	super		( t ),
	m_scene		( NEW( render::scene_ptr) ( scene ) ),
	m_cook_data	(NEW(xray::render::material_effects_instance_cook_data*))
{
}

object_sky::~object_sky( )
{
	DELETE		( m_scene );
	DELETE		( m_cook_data );
	
	m_scene		= NULL;
}

//////////////////////////////////////////			P R O P E R T I E S				 //////////////////////////////////////////////

String^ object_sky::sky_material::get ( )
{
	return m_material_name;
}

void object_sky::sky_material::set ( String^ value )
{
	if( m_material_name != value )
	{
		m_material_name	= value;
	}
	requery_material				( );
}

//////////////////////////////////////			P U B L I C   M E T H O D S				 //////////////////////////////////////////////



void object_sky::save( configs::lua_config_value t )
{
	super::save( t );
	t["game_object_type"]	= "sky";
	t["material_name"]		= unmanaged_string(m_material_name).c_str();
}

void object_sky::load_props( configs::lua_config_value const& t )
{
	super::load_props( t );
	String^ s		= gcnew String( (pcstr)t["material_name"] );
	
	if ( m_material_name != s )
	{
		m_material_name	= s;
		requery_material( );
	}
}

void object_sky::load_contents( )
{
	
}

void object_sky::unload_contents( bool )
{
	
}

void object_sky::requery_material( )
{
	if( m_material_name->Length == 0 )
		return; // no material assigned
	
	unmanaged_string request_path			( m_material_name );
	resources::request request[]			= { request_path.c_str( ), resources::material_effects_instance_class };
	
	resources::user_data_variant* temp_data[] = { NEW(resources::user_data_variant) };
	
	*m_cook_data							= 
		NEW(xray::render::material_effects_instance_cook_data)(
			xray::render::post_process_vertex_input_type,
			NULL,
			false,
			xray::render::cull_mode_none
		);
	
	temp_data[0]->set						(*m_cook_data);
	
	query_result_delegate* query_delegate	= NEW(query_result_delegate)(
		gcnew query_result_delegate::Delegate(this, &object_sky::material_ready), 
		g_allocator
	);
	
	resources::query_resource_params params	(
		request, 
		NULL, 
		1,
		boost::bind(&query_result_delegate::callback, query_delegate, _1),
		g_allocator,
		0,
		0,
		(resources::user_data_variant const**)temp_data
	);
	
 	resources::query_resources		( params );
}

void object_sky::material_ready( resources::queries_result& data )
{
	if( data.is_successful( ) && m_scene && m_scene->c_ptr( ) )
	{
		get_editor_renderer( ).scene( ).set_sky_material( *m_scene, data[0].get_unmanaged_resource( ) );
	}
}

} //namespace editor
} //namespace xray
