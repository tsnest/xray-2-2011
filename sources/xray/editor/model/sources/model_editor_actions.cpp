////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_editor.h"
#include "model_editor_object.h"
#include "ide.h"
#include "property_grid_panel.h"
#include "object_portals_panel.h"
#include "lod_view_panel.h"
#include "input_actions.h"

#pragma managed( push, off )
#include <xray/collision/object.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray{
namespace model_editor{

using editor_base::action_delegate;
using editor_base::execute_delegate_managed;
using editor_base::checked_delegate_managed;
using editor_base::enabled_delegate_managed;


void model_editor::register_actions( )
{
// "View isolate selected"

// "Open"
	System::String^ action_name				= "open solid visual";
	action_delegate^ a						= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &model_editor::action_open_solid_model) );
	m_input_engine->register_action			( a, "" );
	m_gui_binder->add_action_menu_item		( m_ide->top_menu, action_name, "ModelMenuItem", 0);

	action_name								= "open tree visual";
	a										= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &model_editor::action_open_tree_model) );
	m_input_engine->register_action			( a, "" );
	m_gui_binder->add_action_menu_item		( m_ide->top_menu, action_name, "ModelMenuItem", 0);

	action_name								= "new composite visual";
	a										= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &model_editor::action_new_composite_model) );
	m_input_engine->register_action			( a, "" );
	m_gui_binder->add_action_menu_item		( m_ide->top_menu, action_name, "ModelMenuItem", 0);

// Maya style mouse actions 
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_xy	( "view maya move xy",	m_view_window ), "Alt+MButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_z		( "view maya move z",	m_view_window ), "Alt+RButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_rotate_around( "view maya rotate",	m_view_window ), "Alt+LButton(View)" );
	
// Old LE style mouse actions 
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_rotate		( "view LE rotate",		m_view_window ), "Space+LButton+RButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_y_reverse( "view LE move y",	m_view_window ), "Space+RButton(View)" );
	m_input_engine->register_action( gcnew	editor_base::mouse_action_view_move_xz_reverse( "view LE move xz",m_view_window ), "Space+LButton(View)" );

	m_view_window->register_actions			( m_input_engine, m_gui_binder, true );

//// "Camera Focus to origin"
	action_name								= "Camera focus to origin";
	m_input_engine->register_action			( gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &model_editor::action_focus_to_selection) ), "F(Global)" );

	action_name								= "select translate";
	editor_base::editor_control_base^ ec	= gcnew editor_base::transform_control_translation(m_transform_control_helper);
	m_input_engine->register_action			( gcnew	action_select_active_control( action_name, this, ec ), "W(View)") ;
	m_gui_binder->register_image			( "base", action_name, editor_base::tool_translate );
	m_view_window->add_action_button_item	( m_gui_binder, action_name, "ToolToolStrip", 1);

	action_name								= "select rotate";
	ec										= gcnew editor_base::transform_control_rotation(m_transform_control_helper);
	m_input_engine->register_action			( gcnew	action_select_active_control( action_name, this, ec ), "E(View)") ;
	m_gui_binder->register_image			( "base", action_name, editor_base::tool_rotate );
	m_view_window->add_action_button_item	( m_gui_binder, action_name, "ToolToolStrip", 3);

	action_name								= "select scale";
	ec										= gcnew editor_base::transform_control_scaling(m_transform_control_helper);
	m_input_engine->register_action			( gcnew	action_select_active_control( action_name, this, ec ), "R(View)") ;
	m_gui_binder->register_image			( "base", action_name,	editor_base::tool_scale );
	m_view_window->add_action_button_item	( m_gui_binder, action_name, "ToolToolStrip", 2);

	m_input_engine->register_action( gcnew	action_select_active_control( "select none", this, nullptr ), "Escape(View)") ;

	m_input_engine->register_action( gcnew	mouse_action_editor_control	( "manipulator drag axis mode",	this, 0, false )	, "LButton" );

	action_name = "touch";
	m_input_engine->register_action( gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &model_editor::action_touch) ), "LButton(View)" );

	action_name = "manipulator vertex snap mode";
	m_input_engine->register_action			( gcnew editor_base::mouse_action_editor_control_snap(action_name,  gcnew editor_base::active_control_getter(this, &model_editor::get_active_control), editor_base::snap_mode::vertex_snap, m_view_window), "V+LButton(View)" );

	action_name								= "manipulator face snap mode";
	m_input_engine->register_action			( gcnew editor_base::mouse_action_editor_control_snap(action_name, gcnew editor_base::active_control_getter(this, &model_editor::get_active_control), editor_base::snap_mode::face_snap, m_view_window), "C+LButton(View)" );

	execute_action							("select translate");
}

void model_editor::action_new_composite_model( )
{
	if(!close_query())
		return;

	if(m_edit_object)
	{
		m_edit_object->clear_resources	( );
		delete m_edit_object;
	}

	Show( "composite_visual", "" );
}

void model_editor::action_open_solid_model( )
{
	System::String^ selected	= nullptr;

	bool result = editor_base::resource_chooser::choose(	"solid_visual", 
															m_edit_object?m_edit_object->full_name():"", 
															nullptr, 
															selected,
															true );

	if(result)
	{
		if(!close_query())
			return;

		if(m_edit_object)
		{
			m_edit_object->clear_resources	( );
			delete m_edit_object;
		}

		Show( "solid_visual", selected );
	}
}

void model_editor::action_open_tree_model( )
{
	System::String^ selected	= nullptr;

	bool result = editor_base::resource_chooser::choose(	"speedtree", 
															m_edit_object?m_edit_object->full_name():"", 
															nullptr, 
															selected,
															true );

	if(result)
	{
		if(!close_query())
			return;

		if(m_edit_object)
		{
			m_edit_object->clear_resources	( );
			delete m_edit_object;
		}

		Show( "speedtree", selected );
	}
}

void model_editor::action_focus_to_selection( )
{
	if(m_edit_object)
	{
		math::aabb const foced_bbox	= m_edit_object->focused_bbox( );
		m_view_window->action_focus_to_bbox( foced_bbox );
	}else
		m_view_window->action_focus_to_bbox( math::create_identity_aabb() );
}


void model_editor::action_touch( )
{
	if( !m_view_window->is_mouse_in_view() )
		return;
	collision::object const* object		= NULL;
	if( m_view_window->ray_query( editor_base::collision_object_type_touch, &object, NULL ) )
	{
		object->touch( );
	}
	else
	{
		if( m_edit_object )
			m_edit_object->reset_selection( );

		if ( m_transform_control_helper->m_object != nullptr )
		{
			m_transform_control_helper->m_object = nullptr;
			m_transform_control_helper->m_changed = true;
		}
	}
}

void model_editor::execute_action( System::String^ action_name )
{
	editor_base::action^ a = m_input_engine->get_action( action_name );
	if(a && a->enabled())
		a->do_it();
}

} //namespace model_editor
} //namespace xray
