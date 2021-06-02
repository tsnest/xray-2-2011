#include "pch.h"
#include "object_painter_tool_tab.h"
#include "level_editor.h"
#include "project.h"
#include "actions.h"
#include "object_solid_visual.h"
#include "object_painter_control.h"

namespace xray{
namespace editor{

void object_painter_tool_tab::post_constructor( )
{
	m_dest_object								= gcnew painted_object;
	m_object_painter_control					= gcnew object_painter_control( m_level_editor, this );
	m_object_painter_control->set_source		( m_dest_object );
	m_level_editor->register_editor_control		( m_object_painter_control );
	m_level_editor->get_input_engine()->register_action( gcnew action_select_editor_control( "select object painter", m_level_editor, "object_painter" ) , "");
	m_object_painter_control->initialize	( );

	xray::editor_base::gui_binder^ binder	= m_level_editor->get_gui_binder();

	ToolStripButton^ button		= nullptr;
	System::String^ action_name	= nullptr;

	action_name					= "select object painter";
	button						= gcnew ToolStripButton;
	top_tool_strip->Items->Add	( button );
	binder->bind				( button, action_name );
	button->Image				= binder->get_image( action_name );
	button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Text;
	button->Text				= "P";
	button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
}

object_painter_tool_tab::~object_painter_tool_tab()
{
	m_level_editor->unregister_editor_control( m_object_painter_control );
	delete									m_object_painter_control;
	m_dest_object->reset					( );
	delete									m_dest_object;
}

void object_painter_tool_tab::add_from_selection_button_Click( System::Object^ , System::EventArgs^ )
{
	m_snap_list_view->Items->Clear	( );

	project_items_list^	selection_list	= m_level_editor->get_project()->selection_list();
	
	project_item_base^ selected = nullptr;

	for each (project_item_base^ item in selection_list)
	{
//		project_item_object^ item_object = item->get_object();
		if( item->get_object() )
		{
			object_base^ o		= item->get_object();
			if(o->GetType() == object_solid_visual::typeid)
			{
				selected		= item;
			}
		}
	}

	if( selected )
	{
		m_snap_list_view->Items->Add	( selected->get_full_name() );
		m_dest_object->setup			( safe_cast<object_solid_visual^>(selected->get_object()));
	}
}

void object_painter_tool_tab::remove_button_Click( System::Object^ , System::EventArgs^ )
{
	System::Windows::Forms::ListView::SelectedListViewItemCollection^ selection = m_snap_list_view->SelectedItems;
	for each( System::Windows::Forms::ListViewItem^ item in selection )
	{
		m_snap_list_view->Items->Remove( item );
	}
}

void object_painter_tool_tab::clear_button_Click( System::Object^ , System::EventArgs^ )
{
	m_snap_list_view->Items->Clear	( );
	m_dest_object->reset			( );
}

void object_painter_tool_tab::focus_button_Click( System::Object^ , System::EventArgs^ )
{
	if( m_dest_object->ready() )
	{
		math::float3 position			= m_dest_object->get_position();
		float distance					= m_level_editor->focus_distance();
		m_level_editor->focus_camera_to	( position, distance );
	}
}

void object_painter_tool_tab::on_activate()
{
	m_level_editor->set_active_tool( nullptr );
}

void object_painter_tool_tab::on_deactivate()
{}

void object_painter_tool_tab::load_settings( RegistryKey^ key )
{
	m_object_painter_control->load_settings( key );
}

void object_painter_tool_tab::save_settings( RegistryKey^ key )
{
	m_object_painter_control->save_settings( key );
}

} //namespace xray
} //namespace editor
