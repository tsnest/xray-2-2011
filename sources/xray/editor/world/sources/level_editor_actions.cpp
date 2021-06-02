////////////////////////////////////////////////////////////////////////////
//	Created		: 22.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "level_editor.h"
#include "project.h"
#include "scene_graph_tab.h"
#include "object_inspector_tab.h"
#include "animation_curve_editor_panel.h"
#include "triangulation_tester.h"
#include "property_connection_editor.h"
#include "project_tab.h"
#include "tools_tab.h"
#include "tool_base.h"
#include "object_base.h"
#include "command_add_library_object.h"
#include "command_paste_objects.h"
#include "command_delete_object.h"
#include "command_select.h"
#include "command_reparent_items.h"
#include "actions.h"
#include "input_actions.h"
#include <xray/editor/base/collision_object_types.h>
#include <xray/editor/base/transform_control_defines.h>
#include "project_browser_dialog.h"
#include "le_transform_control_helper.h"
#include "window_ide.h"
#include "editor_world.h"

#pragma managed( push, off )
#	include <xray/strings_stream.h>
#	include <xray/collision/collision_object.h>
#pragma managed( pop )

namespace xray{
namespace editor{

typedef xray::editor_base::action_delegate action_delegate;
typedef xray::editor_base::execute_delegate_managed execute_delegate_managed;
typedef xray::editor_base::enabled_delegate_managed enabled_delegate_managed;
typedef xray::editor_base::checked_delegate_managed checked_delegate_managed;

void level_editor::register_actions( )
{
	xray::editor_base::gui_binder^ gui_binder = get_gui_binder( );
	ide()->add_menu_item		( gui_binder, "ProjectMenuItem", "Project", 1);
	ide()->create_tool_strip	( "ToolToolStrip", 1);

	System::String^ action_name = "focus";
	get_input_engine()->register_action( gcnew action_focus( action_name, this, false ), "F(Global)");

	action_name = "focus fixed distance";
	get_input_engine()->register_action( gcnew action_focus( action_name, this, true ), "Shift+F(Global)");

	action_name = "select none";
	get_input_engine()->register_action( gcnew action_select_editor_control(action_name, this, nullptr), "Q(Global)");
	gui_binder->register_image( "base", action_name,		xray::editor_base::tool_select );
	ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 0);

	action_name = "select translate";
	get_input_engine()->register_action( gcnew action_select_editor_control(action_name, this, "translation" ),	"W(Global)");
	gui_binder->register_image( "base", action_name,	xray::editor_base::tool_translate );
	ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 1);

	action_name = "select scale";
	get_input_engine()->register_action( gcnew action_select_editor_control(action_name, this, "scale" ), "R(Global)");
	gui_binder->register_image( "base", action_name,		xray::editor_base::tool_scale );
	ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 3);

	action_name = "select rotate";
	get_input_engine()->register_action( gcnew action_select_editor_control(action_name, this, "rotation" ), "E(Global)");
	gui_binder->register_image( "base", action_name,	xray::editor_base::tool_rotate );
	ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 2);

	action_delegate^ a	= nullptr;
	action_name			= "toggle debug renderer";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::toggle_debug_renderer_enabled) );
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::debug_renderer_enabled));
	get_input_engine()->register_action( a ,"");
	gui_binder->register_image( "base", action_name,	xray::editor_base::debug_draw );
	ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 10);

	action_name = "manipulator drag axis mode";
	get_input_engine()->register_action( gcnew	mouse_action_editor_control(action_name, this, 0, false ), "LButton(View)" );

	action_name = "manipulator drag plane mode";
	get_input_engine()->register_action( gcnew	mouse_action_editor_control(action_name, this, 0, true), "Control+LButton(View)|Shift+LButton(View)" );

	action_name = "manipulator free drag axis mode";
	get_input_engine()->register_action( gcnew	mouse_action_editor_control(action_name, this, 1, false ), "MButton(View)" );

	action_name = "manipulator free drag plane mode";
	get_input_engine()->register_action( gcnew	mouse_action_editor_control(action_name, this, 1, true ), "Control+MButton(View)" );

	action_name = "manipulator vertex snap mode";
	get_input_engine()->register_action( gcnew	xray::editor_base::mouse_action_editor_control_snap(action_name,  gcnew xray::editor_base::active_control_getter(this, &level_editor::get_active_control), xray::editor_base::snap_mode::vertex_snap, view_window()), "V+LButton(View)" );

	action_name = "manipulator face snap mode";
	get_input_engine()->register_action( gcnew	xray::editor_base::mouse_action_editor_control_snap(action_name, gcnew xray::editor_base::active_control_getter(this, &level_editor::get_active_control), xray::editor_base::snap_mode::face_snap, view_window()), "C+LButton(View)" );

	action_name = "manipulator step mode";
	get_input_engine()->register_action			( gcnew xray::editor_base::mouse_action_editor_control_snap(action_name,  gcnew xray::editor_base::active_control_getter(this, &level_editor::get_active_control), xray::editor_base::snap_mode::step_snap, view_window()), "X+LButton(View)" );

	action_name = "terrain control radius and power";
	get_input_engine()->register_action( gcnew mouse_action_control_properties(action_name, this, "radius_and_power", 0.05f ), "RButton(View)" );

	action_name = "terrain control smooth power";
	get_input_engine()->register_action( gcnew mouse_action_control_properties(action_name, this, "smooth_power", 0.05f ), "Shift+RButton(View)" );

	action_name = "terrain control dropper";
	get_input_engine()->register_action( gcnew action_control_properties(action_name, this, "dropper" ), "Q(View)" );

	// Object select mouse action
	action_name = "touch";
	get_input_engine()->register_action( gcnew	xray::editor_base::mouse_action_touch_object(action_name, view_window()),						"LButton(View)" );

	action_name		= "flush selection";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::flush_selection) );
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a ,"Escape(Global)");

	action_name = "select";
	get_input_engine()->register_action( gcnew	mouse_action_select_object(action_name, this, enum_selection_method_set ),		"LButton(View)" );

	action_name = "select invert";
	get_input_engine()->register_action( gcnew	mouse_action_select_object(action_name, this, enum_selection_method_invert ),	"Shift+LButton(View)" );

	action_name = "select subtract";
	get_input_engine()->register_action( gcnew	mouse_action_select_object(action_name, this, enum_selection_method_subtract ), "Control+LButton(View)" );

	action_name = "select add";
	get_input_engine()->register_action( gcnew	mouse_action_select_object(action_name, this, enum_selection_method_add ),		"Control+Shift+LButton(View)" );

	action_name = "select all";
	a									= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::select_all) );
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name, "EditMenuItem", 15);

	action_name = "duplicate";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::duplicate_selected) );
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a , "Control+D");
	ide()->add_action_menu_item ( gui_binder, action_name, "EditMenuItem", 10);

	action_name		= "delete in logic hypergraph";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::delete_selected_in_logic_hypergraph));
	get_input_engine()->register_action( a, "Delete(LogicView)" );


	action_name		= "delete";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::delete_selected));
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a, "Delete" );
	ide()->add_action_menu_item ( gui_binder, action_name, "EditMenuItem", 14);

	action_name		= "copy";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::copy_selected_to_clipboard));
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a, "Control+C" );
	ide()->add_action_menu_item ( gui_binder, action_name, "EditMenuItem", 12);

	action_name		= "cut";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::cut_selected_to_clipboard));
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a, "Control+X" );
	ide()->add_action_menu_item ( gui_binder, action_name, "EditMenuItem", 11);

	action_name		= "paste";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::paste_from_clipboard));
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::clipboard_not_empty));
	get_input_engine()->register_action( a, "Control+V" );
	ide()->add_action_menu_item ( gui_binder, action_name, "EditMenuItem", 13);

	action_name		= "add library object";
 	get_input_engine()->register_action( gcnew input_action_add_library_object(action_name, this ), "A+LButton(View)" );

	//action_name		= "snap objects";
	//get_input_engine()->register_action( gcnew key_action_snap(action_name, this ), "X+RButton(View)" );

	action_name		= "group selection";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::group_selection));
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a, "Control+G" );

	action_name		= "ungroup selection";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::ungroup_selection));
	a->set_enabled	(gcnew enabled_delegate_managed(this, &level_editor::selection_not_empty));
	get_input_engine()->register_action( a, "Control+U" );

	//action_name		= "place objects";
	//get_input_engine()->register_action( gcnew key_action_place(action_name, this ), "X+LButton(View)" );

	action_name		= "new project";
	get_input_engine()->register_action( gcnew action_delegate(action_name, gcnew  execute_delegate_managed(this, &level_editor::new_project_action) ), "Control+N" );
	ide()->add_action_menu_item	( gui_binder, action_name, "ProjectMenuItem", 0);

	action_name		= "save project";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::save_project_action));
	a->set_enabled	( gcnew enabled_delegate_managed(this, &level_editor::save_needed) );
	get_input_engine()->register_action( a, "Control+S" );
	ide()->add_action_menu_item ( gui_binder, action_name, "ProjectMenuItem", 2);


	action_name		= "save project as";
	a				= gcnew action_delegate(action_name, gcnew  execute_delegate_managed(this, &level_editor::save_as_project_action));
	a->set_enabled(gcnew enabled_delegate_managed(this, &level_editor::project_not_empty));
	get_input_engine()->register_action( a, "" );
	ide()->add_action_menu_item ( gui_binder, action_name, "ProjectMenuItem", 3);

	action_name		= "Export all as OBJ";
	a				= gcnew action_delegate(action_name, gcnew  execute_delegate_managed(this, &level_editor::export_project_as_obj_action));
	get_input_engine()->register_action( a, "" );
	ide()->add_action_menu_item ( gui_binder, action_name, "FileMenuItem", 10);

	action_name		= "Export selected as OBJ";
	a				= gcnew action_delegate(action_name, gcnew  execute_delegate_managed(this, &level_editor::export_selection_as_obj_action));
	get_input_engine()->register_action( a, "" );
	ide()->add_action_menu_item ( gui_binder, action_name, "FileMenuItem", 11);

	action_name		= "open project";
	get_input_engine()->register_action( gcnew action_delegate(action_name, gcnew  execute_delegate_managed(this, &level_editor::open_project_action) ), "Control+O" );
	ide()->add_action_menu_item ( gui_binder, action_name, "ProjectMenuItem", 1);

	action_name		= "close project";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::close_project_action) );
	a->set_enabled(gcnew enabled_delegate_managed(this, &level_editor::project_not_empty));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name, "ProjectMenuItem", 4);

	get_input_engine()->register_action( gcnew action_drop_objects	( "drop", this ), "D(View)" );

	action_name = "object inspector";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::switch_object_inspector));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::object_inspector_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name,	"ViewMenuItem", 10);

	action_name		= "connection editor";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::switch_connection_editor));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::connection_editor_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name, "ViewMenuItem", 11);

	action_name		= "scene graph";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::switch_scene_graph));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::scene_graph_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name, "ViewMenuItem", 12);

	action_name		= "project explorer";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::show_project_explorer));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::project_explorer_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item( gui_binder, action_name, "ViewMenuItem", 13);

	action_name		= "tools frame";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::show_tools_frame));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::tools_frame_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name, "ViewMenuItem", 14);

	action_name		= "pin selection";
	a				=  gcnew action_delegate(action_name, gcnew  execute_delegate_managed(m_project, &project::pin_selection) );
	a->set_checked	(gcnew checked_delegate_managed(m_project, &project::get_pin_selection));
	get_input_engine()->register_action( a , "Control+P");
	gui_binder->register_image( "base", action_name,	xray::editor_base::pin );
	ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 4);

	action_name		= "edit object pivot";
	a				=  gcnew action_delegate(action_name, gcnew  execute_delegate_managed(&xray::editor_base::transform_control_object::switch_edit_pivot_mode) );
	a->set_checked	(gcnew checked_delegate_managed(&xray::editor_base::transform_control_object::is_edit_pivot_mode));
	get_input_engine()->register_action( a , "Insert");

	//action_name		= "transform group mode";
	//a				=  gcnew action_delegate(action_name, gcnew  execute_delegate_managed(&object_transform_data::switch_edit_group_mode) );
	//a->set_checked	(gcnew checked_delegate_managed(&object_transform_data::is_edit_group_mode));
	//get_input_engine()->register_action( a , "Shift+Insert");
	//gui_binder->register_image( "base", action_name, xray::editor_base::node_group );
	//ide()->add_action_button_item( gui_binder, action_name, "ToolToolStrip", 6);

	action_name		= "animation curve editor";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::switch_animation_curve_editor));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::animation_curve_editor_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name,	"ViewMenuItem", 16);

	action_name		= "camera view panel";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::switch_camera_view_panel));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::camera_view_panel_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name,	"ViewMenuItem", 17);

	action_name		= "trinagulation tester";
	a				= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::switch_triangulation_tester));
	a->set_checked	(gcnew checked_delegate_managed(this, &level_editor::triangulation_tester_visible));
	get_input_engine()->register_action( a , "");
	ide()->add_action_menu_item ( gui_binder, action_name,	"ViewMenuItem", 18);

	action_name		= "toggle console";
	get_input_engine()->register_action( gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::toggle_console_action)), "Oemtilde(View)");


	action_name						= "toggle culling view";
	a								= gcnew action_delegate(action_name, gcnew execute_delegate_managed(this, &level_editor::toggle_culling_view) );
	a->set_checked					(gcnew checked_delegate_managed(this, &level_editor::get_culling_view));
	get_input_engine()->register_action( a, "Ctrl+U(View)");
	gui_binder->register_image		( "base", action_name,	xray::editor_base::show_render_statistics );
	ide()->add_action_button_item	( gui_binder, action_name, "RenderOptionsStrip", 3);

	action_name		= "discrete modify +X axis";
	action_control_step_modify^	act	= gcnew action_control_step_modify( action_name, this, xray::editor_base::enum_transform_axis_x, 1.0f );
	get_input_engine()->register_action( act ,"Z+Right(View)");

	action_name		= "discrete modify -X axis";
	act				= gcnew action_control_step_modify( action_name, this, xray::editor_base::enum_transform_axis_x, -1.0f );
	get_input_engine()->register_action( act ,"Z+Left(View)");

	action_name		= "discrete modify +Y axis";
	act				= gcnew action_control_step_modify( action_name, this, xray::editor_base::enum_transform_axis_y, 1.0f );
	get_input_engine()->register_action( act ,"X+Right(View)");

	action_name		= "discrete modify -Y axis";
	act				= gcnew action_control_step_modify( action_name, this, xray::editor_base::enum_transform_axis_y, -1.0f );
	get_input_engine()->register_action( act ,"X+Left(View)");

	action_name		= "discrete modify +Z axis";
	act				= gcnew action_control_step_modify( action_name, this, xray::editor_base::enum_transform_axis_z, 1.0f );
	get_input_engine()->register_action( act ,"C+Right(View)");

	action_name		= "discrete modify -Z axis";
	act				= gcnew action_control_step_modify( action_name, this, xray::editor_base::enum_transform_axis_z, -1.0f );
	get_input_engine()->register_action( act ,"C+Left(View)");
}


void level_editor::add_library_object( float3 position, tool_base^ tool, System::String^ library_name )
{
	get_command_engine()->run( gcnew command_add_library_object ( this, tool, library_name, create_translation(position), gcnew object_added_callback( tool, &tool_base::on_library_object_created ), true ) );
}

void level_editor::copy_selected_to_clipboard( )
{
	project_items_list^ items = get_project()->selection_list();

	if( items->Count )
	{
		configs::lua_config_ptr config				= configs::create_lua_config( );
		configs::lua_config_value root		= config->get_root();

		for each (project_item_base^ itm in items)
		{
			itm->save								( root, false, false );
			R_ASSERT(itm->m_guid!=System::Guid::Empty);
			unmanaged_string sguid(itm->m_guid.ToString());
			root["entrypoint"][sguid.c_str()] = itm->get_object() ? "object" : "folder";
		}

//		config->save_as				("c:/copied.lua");
		strings::stream stream		(g_allocator);
		config->save				( stream );
		stream.append				( "%c", 0 );

		os::copy_to_clipboard		( (pcstr)stream.get_buffer() );
	}
}

void level_editor::paste_from_clipboard()
{
	pstr string				= os::paste_from_clipboard( *g_allocator );
	// clipboard doesn't contain text information
	if ( !string ) {
		// we could use function os::is_clipboard_empty() to check
		// whether there non-text information and show some message in that case
		return;
	}

	configs::lua_config_ptr config = configs::create_lua_config_from_string( string );
	FREE					( string );
	
	// clipboard does contain text information
	if ( !config ) {
		// but there are no valid lua script in clipboard
		// we could show message about this
		return;
	}

	if ( config->empty() ) {
		// we could show message about this
		return;
	}

	get_command_engine()->run	( gcnew command_paste_objects(this, config->get_root()) );
}

void level_editor::new_project( )
{
	m_project->create_new		();
	get_command_engine()->set_saved_flag();
	ide()->error_message_out	( "new project" );
}

bool level_editor::load( System::String^ name )
{
	if ( !save_confirmation() )
		return			false;

	close_project		();

	set_active_control	( nullptr );

	System::String^ log_string	= System::String::Format("loading project {0}", name );
	ide()->error_message_out	( log_string );

	m_project->load_project		( name );

	get_command_engine()->set_saved_flag();
	change_recent_list	(name, true);
	return				true;
}

bool level_editor::save_project( System::String^ name )
{
	m_project->save_project	( name );
	change_recent_list		( name, true );
	get_command_engine()->set_saved_flag();
	return					true;
}

void level_editor::close_project( )
{
	m_project->clear				();
	get_command_engine()->clear_stack();
}

void level_editor::delete_selected( )
{
	item_id_list^ ids			= gcnew item_id_list;
	project_items_list items	= get_project()->selection_list();

	for each(project_item_base^ itm in items)
	{
		if(!itm->get_persistent())
			ids->Add( itm->id() );
	}
	
	if(ids->Count==0)
		return;

	get_command_engine()->run( gcnew command_delete_objects( this, ids ));
}

void level_editor::delete_selected_in_logic_hypergraph()
{
	
}

void level_editor::cut_selected_to_clipboard()
{
	copy_selected_to_clipboard	( );
	delete_selected				( );
}

void level_editor::duplicate_selected( )
{
	project_items_list^ items = m_project->selection_list();

	if( 0==items->Count )
		return;

	configs::lua_config_ptr cfg			= configs::create_lua_config( );
	configs::lua_config_value root		= cfg->get_root();
	
	for each (project_item_base^ itm in items)
	{
		itm->save ( root, false, false );
		R_ASSERT(itm->m_guid!=System::Guid::Empty);
		unmanaged_string sguid(itm->m_guid.ToString());
		root["entrypoint"][sguid.c_str()] = itm->get_object() ? "object" : "folder";
	}

	get_command_engine()->run		( gcnew command_paste_objects(this, cfg->get_root()));
}

void level_editor::select_all()
{
	project_items_list^ items =	m_project->get_all_items();

	if(items->Count>0)
		get_command_engine()->run( gcnew command_select( m_project, items, enum_selection_method_set ) );
}

bool level_editor::project_not_empty( )
{
	return ( !m_project->empty( ) );
}

bool level_editor::save_needed( )
{
	if(m_project->empty())
		return false;
	else
		return ( m_project->save_needed() ||  !get_command_engine()->saved_flag( ) );
}

editor::object_inspector_tab^				level_editor::get_object_inspector_tab()
{
	return m_object_inspector_tab;
}

editor::animation_curve_editor_panel^		level_editor::get_animation_curve_editor_panel()
{
	return m_animation_curve_editor_panel;
}

editor::triangulation_tester^		level_editor::get_triangulation_tester()
{
	return m_triangulation_tester;
}

editor_base::scene_view_panel^				level_editor::get_camera_view_panel()
{
	return get_editor_world().camera_view_panel();
}

editor::property_connection_editor^			level_editor::get_connection_editor()
{
	return m_connection_editor;
}

editor::tools_tab^							level_editor::get_tools_explorer()
{
	return m_tools_tab;
}

editor::project_tab^						level_editor::get_project_explorer()
{
	return m_project_tab;
}

collision::space_partitioning_tree*			level_editor::get_collision_tree()
{
	return get_editor_world().get_collision_tree();
}


editor::project^							level_editor::get_project()
{
	return m_project;
}

u32											level_editor::selected_items_count()
{
	return get_project()->selection_list()->Count;
}

void level_editor::new_project_action()
{
	if( !save_confirmation		( ) )
		return;

	close_project		( );
	new_project			( );
}

bool level_editor::close_query()
{
	return save_confirmation();
}

void level_editor::open_project_action()
{
	static bool guard = false;
	ASSERT(guard==false);
	guard					= true;
	project_browser_dialog	dlg;

	fs_new::native_path_string resource_path	= fs_new::native_path_string::convert(get_editor_world().engine().get_resources_path());
	System::String^ resource_path_m	= gcnew System::String( resource_path.c_str() );

	if(dlg.SelectProjectToLoad( resource_path_m, m_project->project_name() ) == ::DialogResult::OK)
	{
		ASSERT							( dlg.selected_project );
		load							( dlg.selected_project );
	}
	guard = false;
}

void level_editor::save_project_action ()
{
	if( m_project->name_assigned( ) )
		save_project( m_project->project_name() );
	else
		save_as_project_action( );
}

void level_editor::save_as_project_action()
{
	project_browser_dialog	dlg;

	fs_new::native_path_string resource_path	= fs_new::native_path_string::convert( get_editor_world().engine().get_resources_path() );
	System::String^ resource_path_m	= gcnew System::String( resource_path.c_str() );

	if(dlg.SelectProjectToSave( resource_path_m, m_project->project_name() ) == ::DialogResult::OK)
	{
		ASSERT							( dlg.selected_project );
		save_project					( dlg.selected_project );
	}
}

void level_editor::export_project_as_obj_action()
{
	m_project->export_as_obj( false );
}

void level_editor::export_selection_as_obj_action()
{
	m_project->export_as_obj( true );
}

void level_editor::close_project_action( )
{
	if( !save_confirmation		( ) )
		return;

	close_project		( );
}

void level_editor::ungroup_selection( )
{
	project_items_list^ sel_list	= m_project->selection_list( );
	if(sel_list->Count!=1)
		return;

	project_item_folder^ selected_folder = dynamic_cast<project_item_folder^>(sel_list[0]);
	if(!selected_folder || !selected_folder->m_parent)
		return;

	project_items_list^ items_in_group	= gcnew project_items_list;
	selected_folder->get_all_items		( items_in_group, false );

	string_list^ src_names		= gcnew string_list;
	
	for each(project_item_base^ itm in items_in_group)
		src_names->Add( itm->get_full_name());

	System::String^ parent_folder_name	= selected_folder->m_parent->get_full_name( );
	get_command_engine()->run			( gcnew command_reparent_items( get_project(), src_names, parent_folder_name ) );
	get_command_engine()->run			( gcnew command_delete_objects( this, selected_folder->id() ) );
}

void level_editor::group_selection( )
{
	System::String^ folder_name		= "folder";

	command_add_project_folder^ cmd	= gcnew command_add_project_folder( this, folder_name );
	get_command_engine()->run		( cmd );

	project_item_folder^ folder	= safe_cast<project_item_folder^>(project_item_base::object_by_id(cmd->m_added_folder_id) );
	folder_name					= folder->get_full_name( );

	project_items_list^ items	= gcnew project_items_list( get_project()->selection_list() );
	string_list^ src_names		= gcnew string_list;
	
	for each(project_item_base^ itm in items)
		src_names->Add( itm->get_full_name());

	string_list^	to_move		= gcnew string_list;
	get_project()->extract_topmost_items( src_names, to_move );

	items->Add					( folder );
	
	get_command_engine()->run( gcnew command_reparent_items( get_project(), to_move, folder_name ) );
	get_command_engine()->run( gcnew command_select( get_project(), items, enum_selection_method_set) );
}

void level_editor::flush_selection()
{
	get_command_engine()->run( gcnew command_select(get_project()) );
}

bool level_editor::save_confirmation()
{
	System::String^ reason = gcnew System::String("");
	if( has_uncommited_changes( reason ))
	{
		ShowMessageBox( "Uncommited changes : " + reason,
			MessageBoxButtons::OK, 
			MessageBoxIcon::Exclamation );
		return false;
	}

	if( save_needed() )
	{
		::DialogResult result = ShowMessageBox( "Do you want to save changes in '" 
												+ System::IO::Path::GetFileNameWithoutExtension ( m_project->project_name() )
												+ "' project ?", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Exclamation );
		if ( result == ::DialogResult::Yes )
			save_project_action();
		else 
			if ( result == ::DialogResult::Cancel )
				return false;

	}
	return true;
}

bool level_editor::selection_not_empty()
{
	return selected_items_count()!=0;
}

bool level_editor::clipboard_not_empty( )
{
	return !os::is_clipboard_empty();
}

//---------------
void level_editor::switch_scene_graph()
{
	if(!scene_graph_visible())
	{
		m_scene_graph_tab->initialize	();
		ide()->show_tab					(m_scene_graph_tab);
	}else
		m_scene_graph_tab->Hide();
}

bool level_editor::scene_graph_visible()
{
	return m_scene_graph_tab->Visible;
}

//---------------
void level_editor::switch_object_inspector()
{
	if(!object_inspector_visible())
		get_project()->show_object_inspector();
	else
		get_object_inspector_tab()->Hide();
}

bool level_editor::object_inspector_visible()
{
	return get_object_inspector_tab()->Visible;
}

//---------------
void level_editor::show_project_explorer()
{
	if(!project_explorer_visible())
		ide()->show_tab				(m_project_tab);
	else
		m_project_tab->Hide			();
}
bool level_editor::project_explorer_visible()
{
	return get_project_explorer()->Visible;
}

//------------------
void level_editor::switch_connection_editor()
{
	if(!connection_editor_visible())
		get_project()->show_connection_editor();
	else
		get_connection_editor()->Hide();
}

bool level_editor::connection_editor_visible()
{
	return get_connection_editor()->Visible;
}

//------------------

void level_editor::switch_animation_curve_editor( )
{
	if( !animation_curve_editor_visible( ) )
		ide( )->show_tab( m_animation_curve_editor_panel );
	else
		m_animation_curve_editor_panel->Hide( );
}

void level_editor::switch_triangulation_tester ( )
{
	if ( !triangulation_tester_visible( ) )
		ide( )->show_tab( m_triangulation_tester );
	else
		m_triangulation_tester->Hide( );
}

bool level_editor::animation_curve_editor_visible( )
{
	return get_animation_curve_editor_panel( )->Visible;
}

bool level_editor::triangulation_tester_visible( )
{
	return get_triangulation_tester( )->Visible;
}

//------------------

void level_editor::switch_camera_view_panel( )
{
	if( !camera_view_panel_visible( ) )
		ide( )->show_tab( get_editor_world().camera_view_panel() );
	else
		get_editor_world().camera_view_panel()->Hide( );
}

bool level_editor::camera_view_panel_visible( )
{
	return get_camera_view_panel( )->Visible;
}

//-----------------

void level_editor::show_tools_frame()
{
	if(!m_tools_tab->Visible)
		ide()->show_tab				(m_tools_tab);
	else
		m_tools_tab->Hide			();
}

bool level_editor::tools_frame_visible()
{
	return get_tools_explorer()->Visible;
}

void level_editor::toggle_console_action()
{
	get_editor_world().toggle_console();
}

bool level_editor::context_fit( System::String^ context )
{
	if(context=="ToolsView")
	{
		return m_tools_tab->IsActivated;
	}else
	if(context=="ProjectView")
	{
		return m_project_tab->IsActivated;
	}else
		return false;
}

void level_editor::get_contexts_list( System::Collections::Generic::List<System::String^>^ list )
{
	list->Add("ToolsView");
	list->Add("ProjectView");
}

void level_editor::view_panel_drag_enter( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
{
	XRAY_UNREFERENCED_PARAMETER	( sender );
	add_object_to_scene^ data	= dynamic_cast<add_object_to_scene^>(e->Data->GetData(add_object_to_scene::typeid));
	if(data)
	{
		e->Effect = System::Windows::Forms::DragDropEffects::Link;
	}else
	{
		e->Effect = System::Windows::Forms::DragDropEffects::None;
	}
}

void level_editor::view_panel_drag_drop( System::Object^ sender, System::Windows::Forms::DragEventArgs^ e )
{
	XRAY_UNREFERENCED_PARAMETER	( sender );
	add_object_to_scene^ data	= dynamic_cast<add_object_to_scene^>(e->Data->GetData(add_object_to_scene::typeid));
	
	// If the ray doesn't touch the ground then just place the object on the 0 plane.
	float3	origin;
	float3	direction;
	float3	picked_position;

	view_window()->get_mouse_ray	( origin, direction );

	collision::object const* picked_collision = NULL;

	bool bpick = view_window()->ray_query(	xray::editor_base::collision_object_type_dynamic, 
															origin, 
															direction, 
															&picked_collision, 
															&picked_position );

	if( !bpick )
		bpick = xray::editor_base::transform_control_base::plane_ray_intersect(float3( 0, 0, 0 ),	
															float3( 0, 1, 0 ),
															origin,						
															direction,
															picked_position);
	if( bpick && project_not_empty() )
	{
		add_library_object	( picked_position, data->m_tool, data->m_library_item_name );
		m_editor_world.view_window()->Activate();
	}
}

void level_editor::update_object_inspector( )
{
	get_object_inspector_tab()->update();
}

void level_editor::reset_object_inspector( )
{
	get_object_inspector_tab()->reset();
}

} // namespace editor
} // namespace xray
