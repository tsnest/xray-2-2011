////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_environment.h"
#include "object_base.h"
#include "project.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

object_environment::object_environment( tool_base^ t, render::scene_view_ptr const& scene_view ):
	super		( t ),
	m_scene_view( NEW( render::scene_view_ptr) ( scene_view ) )
{
}

object_environment::~object_environment( )
{
	DELETE		( m_scene_view );
	m_scene_view = NULL;
}

void object_environment::requery_material( )
{
	if(m_material_name->Length==0)
		return; // no material assigned
	
	query_result_delegate* q			= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &object_environment::material_ready), g_allocator);
	unmanaged_string s(m_material_name);
	
	resources::request r[]=
	{s.c_str(), resources::material_class };
	
	resources::query_resource_params	params(
		r, 
		NULL, 
		1,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
	);
	
	resources::query_resources		( params );
}

void object_environment::material_ready( resources::queries_result& data )
{
	if(data.is_successful() && m_scene_view && m_scene_view->c_ptr() )
		get_editor_renderer().scene().set_post_process( *m_scene_view, data[0].get_unmanaged_resource());
}

void object_environment::save( configs::lua_config_value t )
{
	super::save(t);
	t["post_effect"]		= unmanaged_string(m_material_name).c_str();
	t["game_object_type"]	= "environment";
}

void object_environment::load_props( configs::lua_config_value const& t )
{
	super::load_props(t);
	System::String^ s		= gcnew System::String((pcstr)t["post_effect"]);
	
	if(m_material_name	!= s)
	{
		m_material_name		= s;
		requery_material	( );
	}
}

void object_environment::load_contents( )
{
	
}

void object_environment::unload_contents( bool )
{
	
}

} //namespace editor
} //namespace xray
