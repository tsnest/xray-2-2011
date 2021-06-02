////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_painter_control_tab.h"
#include "terrain_modifier_painter.h"
#include "tool_terrain.h"
#include "terrain_object.h"
#include "texture_editor.h"
#include "level_editor.h"

using xray::editor::terrain_painter_control_tab;
using xray::editor::terrain_modifier_detail_painter;
using xray::editor_base::activate_control_event;
using xray::editor_base::deactivate_control_event;
using xray::editor_base::property_changed;
using xray::editor_base::editor_control_base;

void terrain_painter_control_tab::in_constructor( )
{
	terrain_modifier_detail_painter^ control = m_tool->m_terrain_modifier_detail_painter;
	control->subscribe_on_activated			( gcnew activate_control_event(this, &terrain_painter_control_tab::on_control_activated) );
	control->subscribe_on_deactivated		( gcnew deactivate_control_event(this, &terrain_painter_control_tab::on_control_deactivated) );
	control->subscribe_on_property_changed	( gcnew property_changed(this, &terrain_painter_control_tab::external_property_changed) );
}

void terrain_painter_control_tab::on_control_activated( editor_control_base^ )
{
	Show						( );
	sync						( true );
}

void terrain_painter_control_tab::on_control_deactivated( editor_control_base^ )
{
	Hide						( );
}

void terrain_painter_control_tab::sync( bool b_sync_ui )
{
	if(m_in_sync)
		return;

	m_in_sync							= true;
	terrain_modifier_detail_painter^ control	= m_tool->m_terrain_modifier_detail_painter;
	ListViewItem^ selected				= (textures_list_view->SelectedItems->Count)?
										textures_list_view->SelectedItems[0] : nullptr;

	if(b_sync_ui)
	{
		textures_list_view->Items->Clear();

		if(m_tool->get_terrain_core())
		{
			for each(System::String^ t in m_tool->get_terrain_core()->m_used_textures)
				textures_list_view->Items->Add(t);

			if(control->active_texture)
			{
				ListViewItem^ found = textures_list_view->FindItemWithText(control->active_texture);
				if(found)
					found->Selected = true;
				else
					control->setup_active_texture( nullptr );

			}

			textures_list_view->Items->Add("<New>");
		}
	}else
	{
		if(selected)
		{
			control->setup_active_texture( selected->Text );
		}else
			control->setup_active_texture( nullptr );
	}

	int layer_id				= base_layer_button->Checked ? 0 : (first_layer_button->Checked ? 1 : 2 );
	control->set_active_layer	( layer_id );

	m_in_sync = false;
}

void terrain_painter_control_tab::sync( System::Object^, System::EventArgs^ )
{
	sync					(false);
}

void terrain_painter_control_tab::external_property_changed( editor_control_base^ )
{
	sync					(true);
}

bool terrain_painter_control_tab::select_texture( System::String^ active_item )
{
	texture_editor_ptr selector	(m_tool->get_level_editor()->get_editor_world());
	selector->selecting_entity			= resource_editor::entity::resource_name;
	selector->selected_name				= active_item;

	if(selector->ShowDialog(this) == DialogResult::OK)
	{
		System::String^ selected				= selector->selected_name;
		terrain_modifier_detail_painter^ control= m_tool->m_terrain_modifier_detail_painter;
		terrain_core^ core						= m_tool->get_terrain_core();
		
		if(!core->m_used_textures.Contains(selected))
		{
			if(active_item=="<New>")
				core->add_texture				( selected );
			else
				core->change_texture			( active_item, selected );
		}
		control->setup_active_texture			( selected );

		return true;
	}else
		return false;
}

void terrain_painter_control_tab::textures_list_view_MouseDown( System::Object^, System::Windows::Forms::MouseEventArgs^ )
{
}

void terrain_painter_control_tab::textures_list_view_MouseClick( System::Object^, System::Windows::Forms::MouseEventArgs^ )
{
	System::Drawing::Point	pt				= textures_list_view->PointToClient(MousePosition);
	ListViewItem^ list_item					= textures_list_view->GetItemAt(pt.X, pt.Y);

	if(!list_item)
	{
		if(textures_list_view->SelectedItems->Count)
			textures_list_view->SelectedItems[0]->Selected = false;

		sync								(false);
		return;
	}else
	{
		System::String^ active_item		= list_item->Text;

		if(active_item=="<New>")
		{
			select_texture(active_item);
			sync					(true); // control->form
		}else
		{
			list_item->Selected			= true;
			sync						(false);
		}
	}
}

void terrain_painter_control_tab::base_layer_button_CheckedChanged(System::Object^ sender, System::EventArgs^ )
{
	System::Windows::Forms::RadioButton^ btn = safe_cast<System::Windows::Forms::RadioButton^>(sender);
	if(btn->Checked)
		sync(false);
}

void terrain_painter_control_tab::textures_list_view_MouseDoubleClick(System::Object^, System::Windows::Forms::MouseEventArgs^)
{		
	if(textures_list_view->SelectedItems->Count == 0)
		return;

	System::String^ active_item	= textures_list_view->SelectedItems[0]->Text;
	
	if(select_texture(active_item))
		sync		(true);

}
