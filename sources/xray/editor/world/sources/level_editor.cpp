////////////////////////////////////////////////////////////////////////////
//	Created		: 18.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "level_editor.h"
#include "project.h"
#include "object_base.h"
#include "editor_world.h"
#include "tools_tab.h"
#include "project_tab.h"
#include "scene_graph_tab.h"
#include "object_inspector_tab.h"
#include "logic_editor_panel.h"
#include "animation_curve_editor_panel.h"
#include "triangulation_tester.h"
#include "object_painter_tool_tab.h"
#include "property_connection_editor.h"
#include "le_transform_control_helper.h"
#include "object_logic.h"

#include "tool_solid_visual.h"
#include "tool_light.h"
#include "tool_misc.h"
#include "tool_composite.h"
#include "tool_terrain.h"
#include "tool_particle.h"
#include "tool_speedtree.h"
#include "tool_scripting.h"
#include "tool_sound.h"
#include "tool_animations.h"

#include "select_library_class.h"
// commands
#include "object_painter_control.h"
#include "window_ide.h"




Microsoft::Win32::RegistryKey^ get_sub_key		(RegistryKey^ root, System::String^ name);

using xray::editor_base::editor_control_base;

static xray::command_line::key	s_world_key		 ("world", "", "", "project for loading");

RegistryKey^ get_sub_key(RegistryKey^ root, System::String^ name)
{
	RegistryKey^ result = root->OpenSubKey(name, true);
	if ( !result )
		result			= root->CreateSubKey(name);

	return result;
}

namespace xray {
namespace editor {

level_editor::level_editor	( editor_world& world )
: m_editor_world		( world ),
  m_object_tools		( gcnew System::Collections::ArrayList ),
  m_active_control		( nullptr ),
  m_tools_loaded		( false ),
  m_editor_initialized  ( false ),
  m_debug_renderer_enabled( true )
{	
	m_name									= "level_editor";
	m_transform_control_helper				= gcnew le_helper( this );
	m_transform_control_helper->m_object	= nullptr;
	view_window()->subscribe_on_camera_move	( gcnew xray::editor_base::camera_moved_delegate(this, &level_editor::on_camera_move));
	view_window()->subscribe_on_drag_drop	( gcnew System::Windows::Forms::DragEventHandler(this, &level_editor::view_panel_drag_drop));
	view_window()->subscribe_on_drag_enter	( gcnew System::Windows::Forms::DragEventHandler(this, &level_editor::view_panel_drag_enter));

}


level_editor::~level_editor( )
{}

editor_base::gui_binder^ level_editor::get_gui_binder( )
{
	return get_editor_world().get_gui_binder();
}

editor_base::command_engine^ level_editor::get_command_engine( )
{
	return get_editor_world().command_engine();
}

editor_base::input_engine^ level_editor::get_input_engine( )
{
	return get_editor_world().input_engine();
}

void level_editor::initialize( )
{
	m_tools_tab						= gcnew tools_tab;
	m_project_tab					= gcnew project_tab(this);
	m_object_inspector_tab			= gcnew object_inspector_tab;
	m_animation_curve_editor_panel	= gcnew animation_curve_editor_panel(this);
	m_triangulation_tester			= gcnew triangulation_tester ( );
	
 	m_project					= gcnew project(this);
	m_project->subscribe_on_selection_changed( gcnew selection_changed(this, &level_editor::on_selection_changed) );
	tool_solid_visual^ tool_sv	= gcnew tool_solid_visual(this);
	m_tools_tab->add_tab		("solid visual", tool_sv->ui()->frame());
	m_object_tools.Add			( tool_sv );

	tool_light^ p_tool_light	= gcnew tool_light(this);
	m_tools_tab->add_tab		("lights", p_tool_light->ui()->frame());
	m_object_tools.Add			( p_tool_light );

	tool_composite^ p_tool_composite= gcnew tool_composite(this);
	m_tools_tab->add_tab		("composite", p_tool_composite->ui()->frame());
	m_object_tools.Add			( p_tool_composite );

	tool_terrain^ p_tool_terrain= gcnew tool_terrain(this);
	m_tools_tab->add_tab		("terrain", p_tool_terrain->ui()->frame());
	m_object_tools.Add			(p_tool_terrain);

	tool_particle^ p_tool_particle= gcnew tool_particle(this);
	m_tools_tab->add_tab		( "particle", p_tool_particle->ui()->frame() );
	m_object_tools.Add			( p_tool_particle );

	tool_speedtree^ tool_st		= gcnew tool_speedtree(this);
	m_tools_tab->add_tab		( "speedtree", tool_st->ui()->frame() );
	m_object_tools.Add			( tool_st );

	tool_misc^ p_tool_misc		= gcnew tool_misc(this);
	m_tools_tab->add_tab		("misc", p_tool_misc->ui()->frame());
	m_object_tools.Add			( p_tool_misc );

	tool_scripting^ p_tool_scripting	= gcnew tool_scripting(this);
//	m_tools_tab->add_tab		("scripting", p_tool_scripting->ui()->frame());
	m_object_tools.Add			( p_tool_scripting );

	tool_sound^ p_tool_sound	= gcnew tool_sound(this);
	m_tools_tab->add_tab		("sound", p_tool_sound->ui()->frame());
	m_object_tools.Add			( p_tool_sound );

	for each(tool_base^ t in m_object_tools)
		t->initialize();

	tool_animations^ p_tool_animations = gcnew tool_animations( this );

	//m_object_painter_tool_tab	= gcnew object_painter_tool_tab( this );
	//m_tools_tab->add_tab		( "object painter", m_object_painter_tool_tab );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(tool_sv) );// static

	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_light) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_composite) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_particle) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_misc) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_scripting) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_sound) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(tool_st) );
	xray::editor_base::resource_chooser::register_chooser( gcnew library_class_chooser(p_tool_animations) );

	m_tools_tab->activate_tab	( 0 );
// Initializing standard set of transformation controls
	m_transform_control_helper->m_collision_tree = get_collision_tree();
	m_transform_control_translation		= gcnew xray::editor_base::transform_control_translation	( m_transform_control_helper );
	m_transform_control_scaling			= gcnew xray::editor_base::transform_control_scaling		( m_transform_control_helper );
	m_transform_control_rotation		= gcnew xray::editor_base::transform_control_rotation		( m_transform_control_helper );

	register_editor_control				( m_transform_control_translation );
	register_editor_control				( m_transform_control_scaling );
	register_editor_control				( m_transform_control_rotation );

	register_actions					( );

//.	m_project->clear					( );
	m_connection_editor					= gcnew property_connection_editor		( this );
	m_scene_graph_tab					= gcnew scene_graph_tab					( this );
}

void level_editor::clear_resources( )
{
	delete							m_object_inspector_tab;
	m_object_inspector_tab			= nullptr;	

	close_project					( );
	unregister_editor_control		( m_transform_control_translation );
	unregister_editor_control		( m_transform_control_scaling );
	unregister_editor_control		( m_transform_control_rotation );
//	delete							m_object_painter_tool_tab;

	for each (tool_base^ tool in m_object_tools)
		tool->clear_resources		( );

	delete m_transform_control_translation;
	delete m_transform_control_scaling;
	delete m_transform_control_rotation;

	for each (tool_base^ tool in m_object_tools)
		delete				tool;

	m_object_tools.Clear	( );

	delete					m_scene_graph_tab;
	delete					m_connection_editor;
	delete					m_project;
	delete					m_project_tab;
}

object_base_list^ level_editor::selected_objects( )
{
	object_base_list^ result		= gcnew object_base_list;
	project_items_list^ items		= m_project->selection_list();

	for each( project_item_base^ item in items)
		if(item->get_object())
			result->Add( item->get_object() );

	return result;
}

object_base^ level_editor::object_by_id( u32 id )
{
	project_item_base^ item		= project_item_base::object_by_id( id );
	R_ASSERT					(item->get_object());
	return item->get_object();
}

tool_base^ level_editor::get_tool( System::String^ tool_name )
{
	for(int i=0; i<m_object_tools.Count; ++i)
	{
		tool_base^ tool = safe_cast<tool_base^>(m_object_tools[i]);
		if(tool->name()==tool_name)
			return tool;
	}

	UNREACHABLE_CODE(return nullptr);
}

ref struct onframe_delegate
{
	onframe_delegate		( execute_delegate^ d, u32 frames ):m_frame(frames), m_delegate(d){}

	u32						m_frame;
	execute_delegate^		m_delegate;
};

void level_editor::add_onframe_delegate( execute_delegate^ d, u32 frames )
{
	m_invoke_on_frame_delegates.Add( gcnew onframe_delegate(d, frames));
}

void level_editor::remove_onframe_delegate( execute_delegate^ user_delegate )
{
	System::Collections::ArrayList delegates_copy;
	for each ( onframe_delegate^ d in m_invoke_on_frame_delegates )
		delegates_copy.Add( d );

	for each ( onframe_delegate^ d in delegates_copy )
	{
		if ( d->m_delegate == user_delegate )
			m_invoke_on_frame_delegates.Remove( d );
	}
}

void level_editor::tick( )
{
	for each (tool_base^ t in m_object_tools)
		t->tick();

	if( get_active_control() )
		get_active_control()->update	( );

//.	get_editor_renderer().debug().draw_origin	( m_editor_world.scene(), float4x4().identity(), 3.0f );

	if(m_invoke_on_frame_delegates.Count)
	{
		System::Collections::ArrayList		current;
		for each(onframe_delegate^ d in m_invoke_on_frame_delegates)
		{
			if( 0 == (--d->m_frame) )
				current.Add	(d);
		}

		for each(onframe_delegate^ d in current)
		{
			d->m_delegate();
			m_invoke_on_frame_delegates.Remove(d);
		}
	}

	if(m_debug_renderer_enabled)
		m_project->render			( );


	m_transform_control_helper->m_editor_renderer = &get_editor_renderer();

	if(get_active_control())
		get_active_control()->draw	( get_editor_world().scene_view().c_ptr(), get_editor_world().view_output_window().c_ptr() );
	

	u32 pending_queries_count	= resources::pending_queries_count();

	if( pending_queries_count==0 && m_project->m_project_loading_timer && !m_project->empty() )
	{
		System::String^ log_string	= System::String::Format("project loaded in {0} sec.", m_project->m_project_loading_timer->get_elapsed_sec());
		ide()->error_message_out	( log_string );

		DELETE						(m_project->m_project_loading_timer);
		m_project->m_project_loading_timer		= NULL;
	}
}

void level_editor::on_project_clear( )
{
	for each(tool_base^ t in m_object_tools)
		t->on_project_clear();
}

bool level_editor::has_uncommited_changes( System::String^% reason )
{
	bool result = false;
	for each(tool_base^ t in m_object_tools)
	{
		if(t->has_uncommited_changes( reason ))
			result = true;
	}
	return result;
}

void level_editor::tool_loaded( tool_base^ t )
{
	LOG_INFO("tool initialized :%s", unmanaged_string(t->name()).c_str());

	for each(tool_base^ t in m_object_tools)
		if(!t->ready())
			return;

	m_tools_loaded	= true;
	
	if(get_project()->empty())
		new_project( );
}

void level_editor::on_editor_renderer_initialized( )
{	
	m_editor_initialized		= true;
	
	xray::fixed_string512		project_path;
	if ( s_world_key.is_set_as_string( &project_path ) )
	{
		System::String^ project_path_m	= gcnew System::String( project_path.c_str() );
		load						( project_path_m );
	}
}

WeifenLuo::WinFormsUI::Docking::IDockContent^ level_editor::find_dock_content(System::String^ type_name)
{
	IDockContent^ result = nullptr;

	if(type_name=="xray.editor.project_tab")
	{
		result = m_project_tab;
	}else
	if(type_name=="xray.editor.tools_tab")
	{
		result = m_tools_tab;
	}else
	if(type_name=="xray.editor.object_inspector_tab")
	{
		result = m_object_inspector_tab;
	}else
	if(type_name=="xray.editor.animation_curve_editor_panel")
	{
		result = m_animation_curve_editor_panel;
	}else
	if(type_name=="xray.editor.triangulation_tester")
	{
		result = m_triangulation_tester;
	}else
	if(type_name=="xray.editor_base.camera_view_panel")
	{
		result = m_editor_world.camera_view_panel();
	}else
	if(result==nullptr)
	{
		unmanaged_string s(type_name);
		LOG_INFO("frame not found %s", s.c_str());
	
	}
	return result;
}

void level_editor::load_settings( )
{
	if ( m_project )
		m_project->load_settings( );
	RegistryKey^ product_key	= base_registry_key::get();
	RegistryKey^ editor_key		= get_sub_key(product_key, m_name);

	get_input_engine()->load(editor_key);

	for each(tool_base^ tool in m_object_tools)
		tool->load_settings	(editor_key);

	m_transform_control_translation->load_settings	( editor_key );
	m_transform_control_scaling->load_settings		( editor_key );
	m_transform_control_rotation->load_settings		( editor_key );
//	m_object_painter_tool_tab->load_settings		( editor_key );

	editor_key->Close		( );
	product_key->Close		( );
}

void level_editor::save_settings( )
{
	m_project->save_settings	( );
	RegistryKey^ product_key	= base_registry_key::get(); 
	RegistryKey^ editor_key		= get_sub_key(product_key, m_name);

	get_input_engine()->save();

	for each(tool_base^ tool in m_object_tools)
		tool->save_settings	(editor_key);

	m_transform_control_translation->save_settings	( editor_key );
	m_transform_control_scaling->save_settings		( editor_key );
	m_transform_control_rotation->save_settings		( editor_key );
//	m_object_painter_tool_tab->save_settings		( editor_key );

	editor_key->Close		( );
	product_key->Close		( );
}

void level_editor::set_default_layout( )
{
	ide()->show_tab			( m_project_tab, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
	ide()->show_tab			( m_tools_tab, WeifenLuo::WinFormsUI::Docking::DockState::DockRight );
	m_object_inspector_tab->Show( m_project_tab->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Bottom, .25f );
}

float level_editor::focus_distance( )
{
	return m_editor_world.focus_distance();
}

void level_editor::on_active_control_changed( editor_control_base^ c )
{
	get_editor_world().on_active_control_changed(c);
}

render::editor::renderer& level_editor::get_editor_renderer()
{
	return get_editor_world().get_renderer();
}

window_ide^ level_editor::ide()
{
	return get_editor_world().ide();
}

void level_editor::focus_camera_to( float3 const& position, float distance )
{
	float4x4 view_inv	= view_window()->get_inverted_view_matrix();
	float4x4 view		= math::invert4x3( view_inv );


	float3 new_pos		= view_inv.k.xyz()*(-distance) + position;
	float4x4 translate	= create_translation( new_pos - view_inv.c.xyz() );

	view_window()->set_view_point_distance( distance );
	view_window()->set_flying_camera_destination( math::invert4x3( view_inv*translate ) );
}

void level_editor::register_editor_control( editor_control_base^ c )
{
	ASSERT(!m_editor_controls.Contains(c));
	m_editor_controls.Add(c);
}

void level_editor::unregister_editor_control( editor_control_base^ c )
{
	ASSERT(m_editor_controls.Contains(c));
	m_editor_controls.Remove(c);
}

editor_control_base^ level_editor::get_active_control()
{
	return m_active_control;
}

editor_control_base^ level_editor::get_editor_control( System::String^ control_id )
{
	for each (editor_control_base^ e in m_editor_controls)
		if(e->id() == control_id)
			return e;

	return nullptr;
}

void level_editor::set_active_control( System::String^ control_id )
{
	editor_control_base^ result		= nullptr;
	if(control_id!=nullptr)
	{
		if(m_active_control && m_active_control->id()==control_id)
			return;

		result = get_editor_control( control_id );
		ASSERT( (result!=nullptr), "The control is not registered !" );
	}

	if( m_active_control )
		m_active_control->activate	(false);

	m_active_control = result;

	if(m_active_control)
		m_active_control->activate	(true);

	on_active_control_changed(m_active_control);
}

#define _recent_list_prefix		"Line"
#define _recent_key_name		"recent files"

void level_editor::get_recent_list(ArrayList^ recent_list)
{
	recent_list->Clear			();

	RegistryKey^ product_key	= base_registry_key::get();
	ASSERT						(product_key);

	RegistryKey^ editor_key		= get_sub_key(product_key, m_name);
	RegistryKey^ recent_key		= get_sub_key(editor_key, _recent_key_name);

	array<System::String^>^	value_names	= recent_key->GetValueNames();

	for each(System::String^ name in value_names)
	{
		if( !name->StartsWith(_recent_list_prefix) )
			continue; 

		System::String^ tmp_value	=	dynamic_cast<System::String^>(recent_key->GetValue(name));
		if( tmp_value != nullptr )
			recent_list->Add	(tmp_value);
	}

	recent_key->Close		();
	editor_key->Close		();
	product_key->Close		();
}

void level_editor::change_recent_list(System::String^ path, bool b_add)
{
	ArrayList					recent_list;
	get_recent_list				(%recent_list);

	if(recent_list.Contains(path))
		recent_list.Remove		(path);

	if(b_add)
		recent_list.Insert			(0, path);

	// save

	RegistryKey^ product_key	= base_registry_key::get();
	ASSERT						(product_key);

	RegistryKey^ editor_key		= get_sub_key(product_key, m_name);

	editor_key->DeleteSubKey	(_recent_key_name, false);

	RegistryKey^ recent_key		= editor_key->CreateSubKey(_recent_key_name);
	ASSERT						(recent_key);

	int i=0;
	for each(System::String^ path in recent_list)
	{
		ASSERT					(path);
		recent_key->SetValue	(_recent_list_prefix + (i++), path);
	}

	recent_key->Close			();
	editor_key->Close			();
	product_key->Close			();
}

void level_editor::on_active_tool_changed( tool_base^ tool )
{
	XRAY_UNREFERENCED_PARAMETERS( tool );
}

void level_editor::set_active_tool( tool_base^ tool )
{
	m_active_tool			= tool;
	on_active_tool_changed	( m_active_tool );
}

void level_editor::on_camera_move( )
{
}

void level_editor::on_selection_changed( )
{
	project_items_list^ sel_list	= m_project->selection_list( );
	if(sel_list->Count==0)
		set_transform_object( nullptr );
	else
	if(sel_list->Count==1 && dynamic_cast<project_item_folder^>(sel_list[0])) // selected folder
		set_transform_object( gcnew folder_transform_data( this, dynamic_cast<project_item_folder^>(sel_list[0]) ) );
	else
		set_transform_object( gcnew object_transform_data( this ) );
}

void level_editor::set_transform_object( xray::editor_base::transform_control_object^ d )
{
	m_transform_control_helper->m_object	= d;
	m_transform_control_helper->m_changed	= true;
}

xray::editor_base::scene_view_panel^ level_editor::view_window( )
{
	return m_editor_world.view_window();
}

xray::editor_base::camera_view_panel^ level_editor::camera_view_panel	( )
{
	return get_editor_world().camera_view_panel();
}

::DialogResult level_editor::ShowMessageBox(	System::String^ message, 
												System::Windows::Forms::MessageBoxButtons buttons, 
												System::Windows::Forms::MessageBoxIcon icon )
{
	return 		System::Windows::Forms::MessageBox::Show( ide()->wnd(), message, "XRay Editor", buttons, icon );
}

void level_editor::on_tree_view_node_double_click	( TreeNode^ node )
{
	if ( node->Tag->GetType() != project_item_folder::typeid ){
		project_item_object^ node_tag = safe_cast<project_item_object^>(node->Tag);
		System::Object^ object_type = node_tag->get_object()->GetType();
		if ( object_type == object_scene::typeid || object_type == object_job::typeid )
		{
			safe_cast<object_logic^>(node_tag->get_object())->show_logic();
		}
	}
}

render::scene_ptr const& level_editor::scene( )
{
	return get_editor_world().scene();
}

render::scene_view_ptr const& level_editor::scene_view( )
{
	return get_editor_world().scene_view();
}

render::render_output_window_ptr const& level_editor::render_output_window( )
{
	return get_editor_world().view_output_window();
}



} // namespace editor
} // namespace xray
