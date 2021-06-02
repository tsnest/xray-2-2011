////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_editor.h"
#include "ide.h"
#include "lod_view_panel.h"
#include "property_grid_panel.h"
#include "edit_object_base.h"
#include "edit_object_speedtree.h"
#include "edit_object_composite_visual.h"
#include "contexts_manager.h"
#include "input_actions.h"

#pragma managed( push, off )
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/collision/api.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/engine/base_classes.h>
#pragma managed( pop )


namespace xray {
namespace model_editor {

model_editor::model_editor( xray::render::world& render_world, 
							xray::editor_base::tool_window_holder^ holder,
							pcstr resources_path )
:m_render_world				( render_world ),
m_holder					( holder ),
m_edit_object				( nullptr ),
m_resources_path			( gcnew System::String(resources_path) )
{
	m_scene					= NEW(render::scene_ptr);
	m_scene_view			= NEW(render::scene_view_ptr);
	m_output_window			= NEW(render::render_output_window_ptr);

	m_ide						= gcnew ide( this );
	m_ide->FormClosing			+= gcnew System::Windows::Forms::FormClosingEventHandler(this, &model_editor::ide_FormClosing);
	m_ide->cancel_button->Click += gcnew System::EventHandler(this, &model_editor::cancel_button_Click);
	m_ide->apply_button->Click	+= gcnew System::EventHandler(this, &model_editor::apply_button_Click);

	m_view_window			= gcnew xray::editor_base::scene_view_panel;

	m_view_window->create_tool_strip		( "ToolToolStrip", 0);
	m_view_window->create_tool_strip		( "MainToolStrip", 1);
	m_gui_binder->add_menu_item				( m_ide->top_menu, "ModelMenuItem", "Model", 0);
	m_gui_binder->add_menu_item				( m_ide->top_menu, "ViewMenuItem", "View", 1);

	math::float3 eye_pos	= float3(0.f, 5.f, -5.f);
	m_view_window->setup_view_matrix( eye_pos, float3(0.f, 0.f, 0.f) );

	m_view_window->m_collision_tree					= &(*(collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 )));

	bool is_loaded_normally = editor::controls::serializer::deserialize_dock_panel_root(
			m_ide, 
			m_ide->main_dock_panel, 
			registry_key_name(),
			gcnew WeifenLuo::WinFormsUI::Docking::DeserializeDockContent(this, &model_editor::find_dock_content),
			false
		);

	if(!is_loaded_normally)
		set_default_layout		( );

	m_input_engine				= gcnew editor_base::input_engine( editor_base::input_keys_holder::ref, gcnew contexts_manager(this) );
 	m_gui_binder				= gcnew editor_base::gui_binder	( m_input_engine );

	editor_base::image_loader		loader;
	m_gui_binder->set_image_list	( "base", loader.get_shared_images16() );

	m_holder->register_tool_window	( this );
	query_create_render_resources	( );
}

void model_editor::query_create_render_resources( )
{
 	render::editor_renderer_configuration render_configuration;
 	render_configuration.m_create_particle_world		= false;
 	
 	System::Int32 hwnd					= m_view_window->view_handle();
 	
 	resources::user_data_variant* temp_data[] = { NEW(resources::user_data_variant), 0, NEW(resources::user_data_variant)};
 	temp_data[0]->set(render_configuration);
 	temp_data[2]->set(*(HWND*)&hwnd);
 	
 	resources::user_data_variant const* data[] = {temp_data[0], temp_data[1], temp_data[2]};
 	
 	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &model_editor::on_render_resources_ready), g_allocator);
 	
 	xray::const_buffer				temp_buffer( "", 1 );
 	resources::creation_request		requests[] = 
 	{
 		resources::creation_request( "model_editor_scene", temp_buffer, resources::scene_class ),
 		resources::creation_request( "model_editor_scene_view", temp_buffer, resources::scene_view_class ),
 		resources::creation_request( "model_editor_render_output_window", temp_buffer, resources::render_output_window_class )
 	};
  	resources::query_create_resources(
  		requests,
  		boost::bind(&query_result_delegate::callback, q, _1),
  		g_allocator,
  		data
  	);

 	DELETE							( temp_data[0] );
 	DELETE							( temp_data[2] );
}

void model_editor::on_render_resources_ready(resources::queries_result& data)
{
 	ASSERT(data.is_successful());
 	
 	*m_scene					= static_cast_resource_ptr<xray::render::scene_ptr>(data[0].get_unmanaged_resource());
 	*m_scene_view				= static_cast_resource_ptr<xray::render::scene_view_ptr>(data[1].get_unmanaged_resource());
 	*m_output_window			= static_cast_resource_ptr<xray::render::render_output_window_ptr>(data[2].get_unmanaged_resource());
 	m_editor_renderer			= &m_render_world.editor_renderer( );
 	m_view_window->setup_scene	( m_scene->c_ptr(), m_scene_view->c_ptr(), *m_editor_renderer, true );

	m_transform_control_helper						= gcnew me_transform_control_helper( this );
	m_transform_control_helper->m_collision_tree	= m_view_window->m_collision_tree;
	m_transform_control_helper->m_editor_renderer	= m_editor_renderer;
	register_actions			( );
}

model_editor::~model_editor( )
{
	m_holder->unregister_tool_window( this );
}

void model_editor::set_default_layout( )
{
	m_view_window->Show					( m_ide->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Document );
}

void model_editor::clear_resources( )
{
	if(m_edit_object)
		m_edit_object->clear_resources		( );

	m_view_window->clear_resources		( );
	DELETE								( m_output_window );
	DELETE								( m_scene_view );
	DELETE								( m_scene );
	m_view_window->clear_scene			( );
	m_editor_renderer					= NULL;

	collision::delete_space_partitioning_tree( m_transform_control_helper->m_collision_tree );

  	delete m_gui_binder;
  	delete m_input_engine;

	delete m_view_window;
	delete m_ide;
	delete m_edit_object;
}

bool model_editor::close_query( )
{
	if(m_edit_object)
		return !m_edit_object->has_changes();
	else 
		return true;
}

System::Windows::Forms::Form^ model_editor::main_form( ) 
{ 
	return m_ide; 
}

void model_editor::tick( )
{
	m_input_engine->execute( );

	m_gui_binder->update_items		( );
	
	if( m_editor_renderer )
	{
	if(m_edit_object)
		m_edit_object->tick	( );

		m_view_window->tick();

		if(m_edit_object && m_active_control)
		{
			m_active_control->update	( );
			m_active_control->draw		( m_scene_view->c_ptr() , m_output_window->c_ptr() );
		}

		m_editor_renderer->scene().set_view_matrix		( *m_scene_view, math::invert4x3(m_view_window->get_inverted_view_matrix()) );
		m_editor_renderer->scene().set_projection_matrix( *m_scene_view, m_view_window->get_projection_matrix() );
		render::viewport_type const viewport		( float2( 0.f, 0.f ), float2( 1.f, 1.f ) );
		m_view_window->render						( );

		m_editor_renderer->draw_scene				( *m_scene, *m_scene_view, *m_output_window, viewport );
	}
}

void model_editor::Show( System::String^ context1, System::String^ context2 )
{
	m_ide->Show	( );
	m_ide->Focus( );

	if( !context2 )
		return;

	if(context1=="solid_visual" || context1=="skinned_visual")
		m_edit_object					= gcnew edit_object_mesh( this, (context1=="skinned_visual") );
	else
	if(context1=="speedtree")
		m_edit_object					= gcnew edit_object_speedtree( this );
	else
	if(context1=="composite_visual")
		m_edit_object					= gcnew edit_object_composite_visual( this );

	m_edit_object->register_actions	( true );

	bool is_loaded_normally = xray::editor::controls::serializer::deserialize_dock_panel_root(
			m_ide, 
			m_ide->main_dock_panel, 
			registry_key_name(),
			gcnew WeifenLuo::WinFormsUI::Docking::DeserializeDockContent(this, &model_editor::find_dock_content),
			true
		);

	if(!is_loaded_normally)
	{
		set_default_layout					( );
		m_edit_object->set_default_layout	( );
	}

	m_edit_object->load		( context2 );
}

System::String^ model_editor::registry_key_name( )
{
	System::String^ key_name = name();

	if(m_edit_object)
	{
		key_name		+= "@";
		key_name		+= m_edit_object->type();
	}
	return key_name;
}

IDockContent^ model_editor::find_dock_content( System::String^ persist_string )
{
	if(persist_string=="xray.editor_base.scene_view_panel")
		return m_view_window;
	else
		return m_edit_object ? m_edit_object->find_dock_content(persist_string) : nullptr;
}

void model_editor::close_internal( )
{
	editor::controls::serializer::serialize_dock_panel_root(m_ide, 
															m_ide->main_dock_panel, 
															registry_key_name() );
	save_settings		( base_registry_key::get() );
	clear_resources		( );
	m_ide->Close		( );
	delete				this;
}

void model_editor::apply_button_Click( System::Object^, System::EventArgs^ )
{
	using namespace System::Windows::Forms;
	if( m_edit_object && m_edit_object->has_changes() )
	{
		DialogResult result = System::Windows::Forms::MessageBox::Show( "Commit changes?", "Model Assembler", MessageBoxButtons::YesNo, MessageBoxIcon::Question );
		if ( result == DialogResult::Yes )
		{
			if(!m_edit_object->save())
				return;
		}
	}
	close_internal( );
}

void model_editor::cancel_button_Click( System::Object^, System::EventArgs^ )
{
	if ( !m_edit_object || !m_edit_object->has_changes() )
	{
		close_internal();
		return;
	}
	using namespace System::Windows::Forms;
	DialogResult result = System::Windows::Forms::MessageBox::Show( "Cancel changes?", "Model Assembler", MessageBoxButtons::YesNo, MessageBoxIcon::Question );
	if ( result == DialogResult::Yes )
	{
		m_edit_object->revert();
		m_active_control = nullptr;
	}
}

void model_editor::ide_FormClosing( System::Object^, System::Windows::Forms::FormClosingEventArgs^  e)
{
	e->Cancel	= true;
}

void model_editor::refresh_caption( )
{
	bool object_loaded =	m_edit_object->complete_loaded();

	m_ide->Text = System::String::Format("Xray Model Assembler : {0}{1}", 
						m_edit_object->full_name(), 
						object_loaded ? (m_edit_object->has_changes() ? " *" : "") : " loading..." );
}

collision::space_partitioning_tree* model_editor::collision_tree::get()
{
	return m_view_window->m_collision_tree;
}

void model_editor::load_settings( RegistryKey^ product_key )
{
	RegistryKey^ editor_key		= xray::base_registry_key::get_sub_key(product_key, "model_assembler");

	m_view_window->load_settings( editor_key );

	editor_key->Close		( );
}

void model_editor::save_settings( RegistryKey^ product_key )
{
	RegistryKey^ editor_key		= xray::base_registry_key::get_sub_key(product_key, "model_assembler");

	m_view_window->save_settings( editor_key );

	editor_key->Close		( );
}

void model_editor::set_active_control( editor_base::editor_control_base^ c )
{
	if( m_active_control )
		m_active_control->activate	(false);

	m_active_control =  c;

	if(m_active_control)
		m_active_control->activate	(true);
}

} //namespace model_editor
} //namespace xray
