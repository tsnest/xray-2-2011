////////////////////////////////////////////////////////////////////////////
//	Created		: 21.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_editor_object.h"
#include "model_editor.h"
#include "ide.h"

namespace xray{
namespace model_editor{

using editor_base::action_delegate;
using editor_base::execute_delegate_managed;
using editor_base::checked_delegate_managed;
using editor_base::enabled_delegate_managed;

model_editor_object::model_editor_object( model_editor^ parent )
:m_model_editor			( parent ),
m_changed				( false ),
m_draw_object_collision	( false ),
m_type_name				( "base" )
{}

void model_editor_object::tick( )
{}

bool model_editor_object::save( )
{
	reset_modified();
	return true;
}

void model_editor_object::revert( )
{
	reset_modified();
}

void model_editor_object::set_modified( )
{
	m_changed				= true;
	m_model_editor->refresh_caption( );
}

void model_editor_object::reset_modified( )
{
	m_changed				= false;
	m_model_editor->refresh_caption( );
}

bool model_editor_object::has_changes( )
{
	return m_changed;
}

void model_editor_object::load( System::String^ model_full_name )
{
	m_full_name = model_full_name;
}

System::String^ model_editor_object::full_name( )
{
	return m_full_name;
}

void model_editor_object::register_actions( bool bregister )
{
	editor_base::input_engine^ input_engine		= m_model_editor->get_input_engine();
	editor_base::gui_binder^ gui_binder			= m_model_editor->get_gui_binder();
	editor_base::scene_view_panel^ view_window	= m_model_editor->m_view_window;
	System::Windows::Forms::MenuStrip^ ide_menu_strip = m_model_editor->get_ide()->top_menu;
	editor_base::action_delegate^ a			= nullptr;

	System::String^ action_name			= "View isolate selected";
	if(bregister)
	{
		a									= gcnew	action_delegate( action_name, gcnew execute_delegate_managed(this, &model_editor_object::action_view_isolate_selected) );
		a->set_checked						( gcnew checked_delegate_managed(this, &model_editor_object::get_view_isolate_selected) );
		input_engine->register_action		( a, "" );
		gui_binder->register_image			( "base", action_name,		editor_base::view_isolate_selected );
		view_window->add_action_button_item	( gui_binder, action_name, "MainToolStrip", 1 );
	}else
	{
		input_engine->unregister_action	( action_name );
		view_window->remove_action_button_item( gui_binder, action_name, "MainToolStrip" );
	}

// "Refresh all materials"
	action_name							= "Refresh all materials";
	if(bregister)
	{
		input_engine->register_action		( gcnew	editor_base::action_delegate( action_name, gcnew execute_delegate_managed( this, &model_editor_object::action_refresh_preview_model) ), "" );
		gui_binder->register_image			( "base", action_name,		editor_base::reload_shaders );
		view_window->add_action_button_item	( gui_binder, action_name, "MainToolStrip", 1 );
	}else
	{
		input_engine->unregister_action	( action_name );
		view_window->remove_action_button_item( gui_binder, action_name, "MainToolStrip" );
	}
	
// "Draw collision"
	action_name							= "Draw object collision";
	if(bregister)
	{
		a									= gcnew	action_delegate( action_name, gcnew execute_delegate_managed( this, &model_editor_object::action_draw_object_collision) );
		a->set_checked						( gcnew checked_delegate_managed( this, &model_editor_object::get_draw_object_collision) );
		input_engine->register_action		( a, "" );
		gui_binder->register_image			( "base", action_name,		editor_base::object_collision );
		view_window->add_action_button_item	( gui_binder, action_name, "MainToolStrip", 1 );
	}else
	{
		input_engine->unregister_action	( action_name );
		view_window->remove_action_button_item( gui_binder, action_name, "MainToolStrip" );
	}

// apply/revert object changes
	action_name							= "Revert changes";
	if(bregister)
	{
		a									= gcnew action_delegate( action_name, gcnew execute_delegate_managed( this, &model_editor_object::revert) );
		a->set_enabled						( gcnew enabled_delegate_managed( this, &model_editor_object::has_changes));
		input_engine->register_action		( a, "" );
		gui_binder->add_action_menu_item	( ide_menu_strip, action_name, "ModelMenuItem", 2);
	}else
	{
		input_engine->unregister_action	( action_name );
		gui_binder->remove_action_menu_item	( ide_menu_strip, action_name, "ModelMenuItem" );
	}

	action_name							= "Apply changes";
	if(bregister)
	{
		a									= gcnew	action_delegate( action_name, gcnew execute_delegate_managed( this, &model_editor_object::action_apply ) );
		a->set_enabled						( gcnew enabled_delegate_managed( this, &model_editor_object::has_changes));
		input_engine->register_action		( a, "" );
		gui_binder->add_action_menu_item	( ide_menu_strip, action_name, "ModelMenuItem", 1);
	}else
	{
		input_engine->unregister_action		( action_name );
		gui_binder->remove_action_menu_item	( ide_menu_strip, action_name, "ModelMenuItem" );
	}
}

void model_editor_object::clear_resources( )
{
	register_actions( false );
}

} // namespace model_editor
} // namespace xray