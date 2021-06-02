////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_composite_visual.h"
#include "property_grid_panel.h"
#include "model_editor.h"
#include "ide.h"

using namespace xray::editor::wpf_controls;

namespace xray{
namespace model_editor{

void edit_object_composite_visual::register_actions( bool bregister )
{
	super::register_actions( bregister );

	System::String^								action_name;
	editor_base::input_engine^ input_engine		= m_model_editor->get_input_engine();
	editor_base::gui_binder^ gui_binder			= m_model_editor->get_gui_binder();
	System::Windows::Forms::MenuStrip^ ide_menu_strip = m_model_editor->get_ide()->top_menu;
	editor_base::action_delegate^ a				= nullptr;

	action_name									= "Contents View";
	if(bregister)
	{
		a										= gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &edit_object_composite_visual::action_contents_panel_view) );
		a->set_checked( gcnew editor_base::checked_delegate_managed	( this, &edit_object_composite_visual::contents_panel_visible));
		input_engine->register_action			( a, "" );
		gui_binder->add_action_menu_item		( ide_menu_strip, action_name, "ViewMenuItem", 0);
	}else
	{
		input_engine->unregister_action			( action_name );
		gui_binder->remove_action_menu_item		( ide_menu_strip, action_name, "ViewMenuItem" );
	}

	action_name		= "edit object pivot";
	if(bregister)
	{
		a										=  gcnew editor_base::action_delegate(action_name, gcnew  editor_base::execute_delegate_managed(&editor_base::transform_control_object::switch_edit_pivot_mode) );
		a->set_checked	(gcnew editor_base::checked_delegate_managed(&editor_base::transform_control_object::is_edit_pivot_mode));
		input_engine->register_action( a , "Insert");
	}else
	{
		input_engine->unregister_action			( action_name );
	}
}

void edit_object_composite_visual::action_draw_object_collision( )
{
	super::action_draw_object_collision();
}

void edit_object_composite_visual::action_view_isolate_selected( )
{
	super::action_view_isolate_selected();
}

void edit_object_composite_visual::action_contents_panel_view( )
{
	//FIXME: copy-paste
	if( contents_panel_visible() )
	{
		m_object_contents_panel->Hide();
	}else
	{
		if( m_object_contents_panel->DockPanel ) 
			m_object_contents_panel->Show	( m_model_editor->get_ide()->main_dock_panel );
		else
			m_object_contents_panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Float );
	}
}

bool edit_object_composite_visual::contents_panel_visible( )
{
	return !m_object_contents_panel->IsHidden;
}

void edit_object_composite_visual::add_visual_clicked( button^ )
{
	System::String^ name			= nullptr;
	if( editor_base::resource_chooser::choose( "solid_visual", 
												nullptr, 
												nullptr, 
												name,
												true)
		)
	{
		composite_visual_item^ item		= gcnew composite_visual_item( this );
		item->model_name				= name;
		m_contents.Add					( item );
		set_modified					( );
		refresh_surfaces_panel			( );
	}
}

void edit_object_composite_visual::remove_item_clicked( button^ bitton )
{
	composite_visual_item^ item		= safe_cast<composite_visual_item^>( bitton->tag );
	m_contents.Remove				( item );

	item->set_selected				( false );

	delete							item;

	set_modified					( );
	refresh_surfaces_panel			( );
}


}
}

