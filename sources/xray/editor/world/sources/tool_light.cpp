////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_light.h"
#include "object_light.h"
#include "light_tool_tab.h"
#include "level_editor.h"
#include "project_items.h"
#include "editor_world.h"
#include "project.h"


#pragma managed( push, off )
#include <xray/geometry_primitives.h>
#include <xray/collision/api.h>
#include <xray/collision/geometry.h>
#include <xray/geometry_utils.h>
#include <xray/geometry_primitives.h>
#pragma managed( pop )

namespace xray{
namespace editor{

tool_light::tool_light(xray::editor::level_editor^ le)
:super(le, "light")
{
	default_image_index	= xray::editor_base::object_light;
	m_tool_tab 			= gcnew light_tool_tab(this);
	load_library		( );

	//int LINE_DIVISION = 32;
	//for(int c=0; c<4; ++c)
	//	for(int i=0;i<LINE_DIVISION;i++)
	//	{
	//		float const angle			= math::pi_x2 * (i / (float)LINE_DIVISION);
	//		math::sine_cosine	sc(angle);
	//		if(c==0)
	//			LOG_INFO("c1 %ff, %ff, %ff,", sc.cosine, sc.sine, 0.0f );// c1
	//		else
	//		if(c==1)
	//			LOG_INFO("c2 %ff, %ff, %ff,", 0.0f, sc.cosine, sc.sine );// c2
	//		else
	//		if(c==2)
	//			LOG_INFO("c3 %ff, %ff, %ff,", sc.sine, 0.0f, sc.cosine );// c3
	//		else
	//		if(c==3 && i!=0)
	//			LOG_INFO("i: %d, %d", i-1, i );// indices
	//	}

	geometry_utils::geom_vertices_type	vertices(g_allocator);
	geometry_utils::geom_indices_type	indices(g_allocator);
	
	geometry_utils::create_octahedron	( vertices, indices, float4x4().identity(), float3(0.2f, 0.2f, 0.2f) );

	vectora<u32>			collision_indices(g_allocator);

	vectora<u16>::const_iterator it		= indices.begin();
	vectora<u16>::const_iterator it_e	= indices.end();
	for( ; it!=it_e; ++it)
		collision_indices.push_back(*it);

	non_null<xray::collision::geometry>::ptr geom = 
	collision::new_triangle_mesh_geometry( g_allocator, 
											(float3*)&vertices[0], 
											vertices.size(), 
											&collision_indices[0], 
											collision_indices.size() );
	m_object_geometry	= &*geom;

}

tool_light::~tool_light			( )							
{
	collision::delete_geometry			( g_allocator, m_object_geometry );
}

void tool_light::on_new_project( render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	float4x4					rotation, translation;
	translation					= float4x4().identity();
	translation.lines[3]		= float4(-1.0f, 9.1f, 4.5f, 1.0f);
	rotation					= math::create_rotation(float3(-math::pi_d3, -math::pi_d3, 0.0f));
	object_base^ o				= create_library_object_instance( "sun", scene, scene_view );
	o->set_transform			( rotation * translation );
	
	project_item_object^ scene_item	= gcnew project_item_object( m_level_editor->get_project(), o );
	scene_item->assign_id		( 0 );
	scene_item->m_guid			= System::Guid::NewGuid();
	m_level_editor->get_project()->root()->add( scene_item );
	o->set_visible				( true );
}

pcstr tool_light::get_library_module_name( )
{
	return "library/lights/lights.library";
}

void tool_light::save_library( )
{
	fs_new::virtual_path_string name	= "resources/";
	name					+= get_library_module_name();
	m_library->save_as	( name.c_str() );
}

editor::tool_tab^ tool_light::ui()
{
	return m_tool_tab;
}

object_light^ tool_light::create_raw_object(System::String^ id, render::scene_ptr const& scene)
{
	object_light^ result = nullptr;
	if(id=="base:light")
		result = gcnew object_light(this, scene);

	ASSERT				( result, "raw object not found", unmanaged_string(id).c_str() );
	return result;
}

object_base^ tool_light::load_object( configs::lua_config_value const& t, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER	( scene_view );

	lib_item^ li				= m_library->extract_library_item( t );
	System::String^ raw			= gcnew System::String( (*(li->m_config))["lib_name"]);
	object_base^ result			= create_raw_object( raw, scene );
	result->load_props			( t );
	result->set_library_name	( li->full_path_name() );

	return						result;
}

object_base^ tool_light::create_library_object_instance( System::String^ library_name, render::scene_ptr const& scene, render::scene_view_ptr const& scene_view )
{
	XRAY_UNREFERENCED_PARAMETER	( scene_view );

	lib_item^ li				= m_library->get_library_item_by_full_path( library_name, true);
	System::String^ raw			= gcnew System::String((*(li->m_config))["lib_name"]);
	object_base^ result			= create_raw_object(raw, scene);
	result->load_props			(*li->m_config);
	result->set_library_name	( library_name );
	result->set_name			( li->m_name, false );

	return						result;
}

void tool_light::destroy_object( object_base^ o )
{
	if(o->get_visible())
		o->set_visible			( false );

	delete						o;
}

void tool_light::load_library( )
{
	fs_new::virtual_path_string res_name = "resources/";
	res_name					+= get_library_module_name();

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &super::on_library_files_loaded), g_allocator);
		resources::query_resource(
		res_name.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void tool_light::on_library_loaded()
{
	super::on_library_loaded	( );
	m_tool_tab->fill_objects_list( );
}

} // namespace editor
} // namespace xray
