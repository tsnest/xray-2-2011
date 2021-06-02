////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_editor.h"
#include "texture_editor_source.h"
#include "texture_document.h"
#include "texture_editor_resource.h"
#include <xray/resources_unmanaged_resource.h>

using WeifenLuo::WinFormsUI::Docking::DockState;
using namespace xray::editor::wpf_controls::property_grid;

namespace xray{
namespace editor{


void texture_editor::in_constructor( )
{
	if(s_alive_editors == 0)
		create_chessboard();

	++s_alive_editors;

	m_multidocument->Name		= "texture_editor";
	m_texture_view_panel		= gcnew controls::file_view_panel_base(m_multidocument);
	m_properties_panel->Text	= "Texture Conversion Options";

	m_texture_view_panel->tree_view->keyboard_search_enabled		= true;
	m_texture_view_panel->tree_view->selected_items_changed			+= gcnew System::EventHandler<controls::tree_view_selection_event_args^>(this, &texture_editor::on_resource_selected);
	FormClosed														+= gcnew FormClosedEventHandler(this, &texture_editor::form_closed);
}

void texture_editor::create_chessboard( )
{
	const int	side_size = 32;//2048;
	//make Chessboard
	s_chessboard					= gcnew Bitmap(side_size, side_size);
	// Lock the bitmap's bits.  
	System::Drawing::Rectangle rect = System::Drawing::Rectangle(0,0,side_size,side_size);
	System::Drawing::Imaging::BitmapData^ bmpData = s_chessboard->LockBits( rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, s_chessboard->PixelFormat );

	// Get the address of the first line.
	System::IntPtr ptr = bmpData->Scan0;

	// Declare an array to hold the bytes of the bitmap.
	// This code is specific to a bitmap with 24 bits per pixels.
	int points = s_chessboard->Width * s_chessboard->Height;
	array<int>^rgbValues = gcnew array<int>(points);

	int value = 0;
	int cell_size_x = 16;
	int cell_size_y = 16;

	for(int x=0; x<side_size; x++)
		for(int y=0; y<side_size; y++)
		{
			value = 255- (( ((x%(2*cell_size_x))-(x%cell_size_x))/cell_size_x + ((y%(2*cell_size_y))-(y%cell_size_y))/cell_size_y)%2) *52;
			rgbValues[ x+y*s_chessboard->Width + 0]	= (255<<24)+(value<<16)+(value<<8)+value;
		}

	// Copy the RGB values back to the bitmap
	System::Runtime::InteropServices::Marshal::Copy( rgbValues, 0, ptr, points );

	// Unlock the bits.
	s_chessboard->UnlockBits( bmpData );

}

void texture_editor::distroy_chessboard( )
{
	delete				s_chessboard;
	s_chessboard		= nullptr;
}

void texture_editor::form_closed( Object^ , FormClosedEventArgs^ )
{
	--s_alive_editors;
	if(s_alive_editors == 0)
		distroy_chessboard();
}

bool texture_editor::load_panels_layout( )
{
	m_multidocument->main_dock_panel->DockRightPortion = 0.40f;
	bool is_loaded_normally = resource_editor::load_panels_layout();

	if(is_loaded_normally)
		m_texture_view_panel->Show				(m_multidocument->main_dock_panel);
	else
	{
		m_multidocument->main_dock_panel->DockLeftPortion = 0.25f;
		m_texture_view_panel->Show				(m_multidocument->main_dock_panel, DockState::DockLeft);
	}

	m_multidocument->is_single_document						= true;
	textures_panel->tree_view->LabelEdit					= false;
	textures_panel->tree_view->ContextMenu					= nullptr;
	textures_panel->tree_view->ImageList					= m_images;
	textures_panel->tree_view->is_selectable_groups			= true;
	textures_panel->tree_view->is_multiselect				= true;

	textures_panel->tree_view->source						= m_textures_source;
	textures_panel->tree_view->refresh						();

	return is_loaded_normally;
}

IDockContent^ texture_editor::layout_for_panel( System::String^ panel_name )
{
	if(panel_name == "xray.editor.controls.file_view_panel_base")
		return m_texture_view_panel;

	return resource_editor::layout_for_panel(panel_name);
}

System::String^ texture_editor::selected_name::get( )
{
	if(selecting_entity == entity::resource_name)
		return textures_panel->tree_view->selected_nodes[0]->FullPath;
	
	R_ASSERT("trying to receive resource name when requesting resource");
	return System::String::Empty;
}

void texture_editor::selected_name::set( System::String^ value )
{
	if(m_is_resource_list_loaded)
		textures_panel->tree_view->track_active_node(value);
	else
	{
		m_track_resource_name = value;
	}
}

document_base^ texture_editor::create_new_document( )
{
	texture_document^ doc		= gcnew texture_document(m_multidocument, this);

	doc->is_use_by_selector		= is_selector;
	doc->resource_loaded		+= gcnew System::EventHandler(this, &texture_editor::resources_loaded);

	return doc;
}

void texture_editor::on_resource_selected( Object^ ,controls::tree_view_selection_event_args^ )
{
	m_selected_resources->Clear	( );
	controls::tree_node^		last_node;

	m_need_view_selected		= true;

	for each(controls::tree_node^ node in textures_panel->tree_view->selected_nodes)
	{
		if(node->m_node_type == controls::tree_node_type::single_item)
		{
			System::String^ node_path	= node->FullPath;

			resource_editor_resource^ res		= nullptr;
			if( m_opened_resources->ContainsKey(node_path) )
				res = m_opened_resources[node_path];

			if(!res)
			{
				res		= gcnew texture_editor_resource( node_path, 
									gcnew resource_changed_delegate(this, &texture_editor::on_resource_changed),
									gcnew resource_load_delegate(this, &texture_editor::on_resource_loaded) );

				m_opened_resources->Add( res->m_name, res );
			}

			m_selected_resources->Add(res);
			last_node			= node;
		}
	}

	if( last_node != nullptr )
		m_multidocument->load_document		( last_node->FullPath );
}

void texture_editor::on_resource_loaded( resource_editor_resource^ resource, bool /*is_correct*/ )
{
	if(m_selected_resources->Contains(resource))
	{
		m_need_view_selected			= true;
		for each( resource_editor_resource^ res in m_selected_resources)
			if(!res->loaded)
				m_need_view_selected	= false;
	}
}

void texture_editor::on_resource_changed( resource_editor_resource^ data )
{
	make_wrapper_modified	( data );
}


void texture_editor::on_menu_opening( System::Object^ sender, System::ComponentModel::CancelEventArgs^ e )
{
	super::on_menu_opening( sender, e );

	bool can_revert_or_apply = false;

	for each(TreeNode^ node in textures_panel->tree_view->selected_nodes)
	{
		if(	node->ImageIndex == xray::editor_base::node_texture_modified)
		{
			can_revert_or_apply = true;
			break;
		}
	}

	revert_toolStripMenuItem->Enabled	= can_revert_or_apply;
	applyToolStripMenuItem->Enabled		= can_revert_or_apply;
}

void texture_editor::tick( )
{
	super::tick		( );

	if(m_need_view_selected)
	{
		view_selected_options_impl				( );
		m_need_view_selected					= false;
	}

	if(m_need_refresh_properties)
	{
		properties_panel->property_grid_control->update();
		m_need_refresh_properties				= false;
	}


}

} // namespace editor
} // namespace xray