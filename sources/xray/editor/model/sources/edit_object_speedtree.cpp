////////////////////////////////////////////////////////////////////////////
//	Created		: 22.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_speedtree.h"
#include "model_editor.h"
#include "property_grid_panel.h"
#include "ide.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace model_editor {
speedtree_surface::speedtree_surface( edit_object_speedtree^ parent, System::String^ name )
:m_parent_object( parent ),
m_name			( name ),
m_material_name	( "" )
{
	m_material = NEW(resources::unmanaged_resource_ptr)();
}

speedtree_surface::~speedtree_surface( )
{
	DELETE( m_material );
}

void speedtree_surface::save_material_settings( configs::lua_config_value& t )
{
	t["material"] = unmanaged_string(material_name).c_str();
}

bool speedtree_surface::load_material_settings( configs::lua_config_value const& t )
{
	bool result			= false;

	if(t.value_exists("material"))
	{
		m_material_name	= gcnew System::String( (pcstr)t["material"] );
		result			= true;
	}

	query_material		( );
	return				result;
}

void speedtree_surface::material_name::set( System::String^ name )
{
	m_material_name					= name;
	query_material					( );
	m_parent_object->set_modified	( );
}

void speedtree_surface::on_selected( bool )
{}

void speedtree_surface::query_material( )
{
	if(m_material_name->Length==0)
	{
		apply_material				( );
		return; // no material assigned
	}
	query_result_delegate* q			= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &speedtree_surface::on_material_ready), g_allocator);
	unmanaged_string s(m_material_name);

	resources::query_resource(
		s.c_str(), 
		resources::material_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator );
}

void speedtree_surface::on_material_ready( resources::queries_result& data )
{
	if(!data.is_successful())
	{
		(*m_material)					= NULL;
		m_parent_object->set_modified	( );
	}else
		(*m_material)					= data[0].get_unmanaged_resource();

	apply_material						( );
}

void speedtree_surface::apply_material( )
{
	if(!m_parent_object->get_model().c_ptr())
		return;

	m_parent_object->get_model_editor()->editor_renderer()->scene().set_speedtree_instance_material( 
		m_parent_object->get_model(), 
		unmanaged_string(name).c_str(),
		*m_material);
}


edit_object_speedtree::edit_object_speedtree( model_editor^ parent )
:super	( parent )
{
	m_type_name					= "speedtree";
	m_speedtree_instance_ptr	= NEW(xray::render::speedtree_instance_ptr)();
	m_model_config				= NEW(configs::lua_config_ptr)();

	speedtree_surface^ s		= nullptr;
	System::String^ sname		= nullptr;
	sname = "branch";
	s = gcnew speedtree_surface( this, sname );
	m_surfaces.Add(sname, s);

	sname = "frond";
	s = gcnew speedtree_surface( this, sname );
	m_surfaces.Add(sname, s);

	sname = "leafmesh";
	s = gcnew speedtree_surface( this, sname );
	m_surfaces.Add(sname, s);

	sname = "leafcard";
	s = gcnew speedtree_surface( this, sname );
	m_surfaces.Add(sname, s);

	sname = "billboard";
	s = gcnew speedtree_surface( this, sname );
	m_surfaces.Add(sname, s);

	m_object_surfaces_panel			= gcnew property_grid_panel( );
}

edit_object_speedtree::~edit_object_speedtree( )
{
	DELETE		( m_model_config );
	DELETE		( m_speedtree_instance_ptr );
	delete		m_object_surfaces_panel;

}

void edit_object_speedtree::tick( )
{
	super::tick();
}

bool edit_object_speedtree::save( )
{
	configs::lua_config_ptr config_ptr	= configs::create_lua_config( );
	configs::lua_config_value& root		= config_ptr->get_root();

	for each( speedtree_surface^ s in m_surfaces.Values )
	{
		unmanaged_string sg_name( s->name );
		configs::lua_config_value current	= root[sg_name.c_str()];
		s->save_material_settings			( current );
	}	
	(*m_model_config)->get_root().assign_lua_value	( config_ptr->get_root() );

	fs_new::virtual_path_string					config_name;

	config_name.assignf				( "resources/speedtree/%s.options", 
										unmanaged_string(m_full_name).c_str() 
										);

	(*m_model_config)->save_as		( config_name.c_str(), configs::target_sources );

	return super::save						( );
}

void edit_object_speedtree::revert( )
{
	load_model_from_config	( );
	super::revert			( );

}

void edit_object_speedtree::load( System::String^ model_full_name )
{
	super::load( model_full_name );

	fs_new::virtual_path_string path	= "resources/speedtree/";
	path.append				( unmanaged_string(model_full_name).c_str() );
	path.append				( ".options" );

	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &edit_object_speedtree::on_model_config_loaded), g_allocator);
	
	resources::query_resource	(
		path.c_str(),
		resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);
}


void edit_object_speedtree::on_model_loaded( resources::queries_result& data )
{
	R_ASSERT( data.is_successful() );
	
 	*m_speedtree_instance_ptr	= static_cast_resource_ptr<render::speedtree_instance_ptr>( data[0].get_unmanaged_resource() );
	m_model_editor->editor_renderer()->scene().add_speedtree_instance( m_model_editor->scene(), *m_speedtree_instance_ptr, math::float4x4().identity(), true );

	action_refresh_preview_model		( );
}

void edit_object_speedtree::load_model_from_config( )
{
	bool load_result = true;
	for each( speedtree_surface^ s in m_surfaces.Values )
	{
		unmanaged_string sn(s->name);
		configs::lua_config_value t		= (*m_model_config)->get_root()[sn.c_str()];
		load_result						&= s->load_material_settings( t );
	}

	if(!load_result)
		set_modified	( );

	refresh_surfaces_panel	( );
}

void edit_object_speedtree::on_model_config_loaded( resources::queries_result& data )
{
	if( data.is_successful() )
	{
		(*m_model_config)			= static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
	}else
		(*m_model_config)			= configs::create_lua_config();

	load_model_from_config			( );
	// query for preview model
	
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &edit_object_speedtree::on_model_loaded), g_allocator);
	
	resources::query_resource	(
		unmanaged_string(full_name()).c_str(),
		resources::speedtree_instance_class,
		boost::bind(&query_result_delegate::callback, q, _1),
		g_allocator
		);

}

void edit_object_speedtree::refresh_surfaces_panel( )
{
	m_object_surfaces_panel->set_property_container( get_surfaces_property_container() );
}

void edit_object_speedtree::clear_resources( )
{
	super::clear_resources	( );
	if(m_speedtree_instance_ptr->c_ptr())
	{
		m_model_editor->editor_renderer()->scene().remove_speedtree_instance( m_model_editor->scene(), *m_speedtree_instance_ptr, true );
		*m_speedtree_instance_ptr = NULL;
	}
}

void edit_object_speedtree::set_default_layout( )
{
	m_object_surfaces_panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
}

bool edit_object_speedtree::complete_loaded( )
{
	return (NULL!=m_speedtree_instance_ptr->c_ptr());
}

void edit_object_speedtree::reset_selection( )
{}

IDockContent^ edit_object_speedtree::find_dock_content( System::String^ persist_string )
{
	if(persist_string=="xray.model_editor.property_grid_panel")
		return m_object_surfaces_panel;
	else
		return nullptr;
}

math::aabb edit_object_speedtree::focused_bbox( )
{
	return math::create_identity_aabb();
}
	
void edit_object_speedtree::register_actions( bool bregister )
{
	super::register_actions( bregister );

	System::String^ action_name;
	editor_base::input_engine^ input_engine		= m_model_editor->get_input_engine();
	editor_base::gui_binder^ gui_binder			= m_model_editor->get_gui_binder();
	System::Windows::Forms::MenuStrip^ ide_menu_strip = m_model_editor->get_ide()->top_menu;
	editor_base::action_delegate^ a				= nullptr;

	action_name										= "Surfaces View";
	if(bregister)
	{
		a											= gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &edit_object_speedtree::action_surfaces_panel_view) );
		a->set_checked( gcnew editor_base::checked_delegate_managed	( this, &edit_object_speedtree::surfaces_panel_visible));
		input_engine->register_action				( a, "" );
		gui_binder->add_action_menu_item			( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action			( action_name );
		gui_binder->remove_action_menu_item		( ide_menu_strip, action_name, "ViewMenuItem" );
	}
}

void edit_object_speedtree::action_surfaces_panel_view( )
{
	//FIXME: copy-paste
	if( surfaces_panel_visible() )
	{
		m_object_surfaces_panel->Hide();
	}else
	{
		if( m_object_surfaces_panel->DockPanel ) 
			m_object_surfaces_panel->Show	( m_model_editor->get_ide()->main_dock_panel );
		else
			m_object_surfaces_panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Float );
	}
}

bool edit_object_speedtree::surfaces_panel_visible( )
{
	return !m_object_surfaces_panel->IsHidden;
}

void edit_object_speedtree::action_refresh_preview_model( )
{
	for each( speedtree_surface^ s in m_surfaces.Values )
		s->apply_material	( );
}

void edit_object_speedtree::action_draw_object_collision( )
{
	super::action_draw_object_collision();
}

void edit_object_speedtree::action_view_isolate_selected( )
{
	super::action_view_isolate_selected();
}


} // namespace model_editor
} // namespace xray
