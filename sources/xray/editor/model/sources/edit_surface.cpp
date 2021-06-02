////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_surface.h"
#include "edit_object_base.h"

namespace xray {
namespace model_editor {

edit_surface::edit_surface( edit_object_mesh^ parent, System::String^ folder_name )
:m_parent_object		( parent ),
m_surface_exported_name	( folder_name ),
m_visible				( true ),
m_selected				( false ),
m_indices				( 999 ),
m_material_query		( NULL )
{
	Triangles				= 0;
	Vertices				= 0;
	m_bbox					= NEW(math::aabb)(math::create_identity_aabb());
	m_preview_material		= NEW(resources::unmanaged_resource_ptr)();
}

edit_surface::~edit_surface( )
{
	DELETE	( m_bbox );
	DELETE	( m_preview_material );
	if(m_material_query)
		m_material_query->m_rejected = true;
}

void edit_surface::query_export_properties( )
{
// query export properties config
	fs_new::virtual_path_string		s;
	s.assignf			("resources/models/%s%s/render/%s/export_properties",
							unmanaged_string(m_parent_object->full_name()).c_str(),
							m_parent_object->model_file_extension(),
							unmanaged_string(m_surface_exported_name).c_str()
							);

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, 
														&edit_surface::on_export_properties_ready), 
														g_allocator);

	resources::query_resource(
		s.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);
}

void edit_surface::on_export_properties_ready( resources::queries_result& data )
{
	R_ASSERT									( data.is_successful() );
	configs::lua_config_ptr	export_props_cfg	= static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	configs::lua_config_value root				= export_props_cfg->get_root();

	m_mesh_name			= gcnew System::String( (pcstr)(root["source"]["mesh_name"]) );
	
	if(root["source"].value_exists("texture_name"))
		m_base_texture_name	= gcnew System::String( (pcstr)(root["source"]["texture_name"]) );

	if(root["source"].value_exists("primitives"))
	{
		Triangles		= root["source"]["primitives"];
		Vertices		= root["source"]["vertices"];
		m_indices		= root["source"]["indices"];
	}
	m_bbox->min			= root["bounding_box"]["min"];
	m_bbox->max			= root["bounding_box"]["max"];

	m_parent_object->refresh_properties_in_grid( );
}

void edit_surface::load_material_settings( configs::lua_config_value const& cfg )
{
	System::String^ s		= gcnew System::String((pcstr)cfg["material_name"]);

	if(m_material_name	!= s)
	{
		m_material_name		= s;
		requery_material	( );
	}
}

void edit_surface::save_material_settings( configs::lua_config_value& cfg )
{
	cfg["material_name"]	= unmanaged_string(m_material_name).c_str();
}

void edit_surface::setup_default_material_settings( )
{
	m_material_name		= "";
}

void edit_surface::material_name::set( System::String^ value )
{
	if(m_material_name	!= value)
	{
		m_material_name				= value;
		m_parent_object->set_modified	( );
	}
	requery_material				( );
}

void edit_surface::Visible::set( bool value )
{
	if(m_visible != value)
	{
		m_visible = value;
		m_parent_object->refresh_surfaces_visibility();
	}
}

void edit_surface::Selected::set( bool value )
{
	if(m_selected != value)
	{
		m_selected = value;
		m_parent_object->refresh_surfaces_visibility();
	}

	if(m_hypergraph_node)
		m_hypergraph_node->hypergraph->select_node( m_hypergraph_node->id );
}

void edit_surface::requery_material( )
{
	if(m_material_name->Length==0)
	{
		(*m_preview_material)				= NULL;
		setup_default_material_settings		( );
		m_parent_object->refresh_properties_in_grid();
		m_parent_object->set_modified			( );
	}

	m_material_query			= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &edit_surface::material_ready), g_allocator);
	unmanaged_string s(m_material_name);

	resources::query_resource( 
		s.c_str(), 
		resources::material_class,
		boost::bind(&query_result_delegate::callback, m_material_query, _1),
		g_allocator
	);

}

void edit_surface::material_ready( resources::queries_result& data )
{
	m_material_query						= NULL;
	if(!data.is_successful())
	{
		LOG_ERROR("material loading FAILED");
		(*m_preview_material)				= NULL;
		setup_default_material_settings		( );
		m_parent_object->refresh_properties_in_grid();
		m_parent_object->set_modified			( );
	}else
		(*m_preview_material)				= data[0].get_unmanaged_resource();

	m_parent_object->apply_surface_material	( name, (*m_preview_material) );
}

void edit_surface::on_selected( System::Boolean value )
{
	Selected = value;
}

void edit_surface::next_lod::set( edit_surface^ s )
{
	m_next_lod_surface			= s;
	m_parent_object->set_modified	( );
}

} // namespace model_editor
} // namespace xray
