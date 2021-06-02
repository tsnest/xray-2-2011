#include "pch.h"
#include "property_connection_editor.h"
#include "object_base.h"
#include "project.h"
#include "command_create_remove_link.h"
#include "level_editor.h"

#include "scene_graph_node.h"

using xray::editor::property_connection_editor;
using xray::editor::object_base;
using System::Drawing::Font;
using System::Drawing::FontStyle;

using xray::editor::scene_graph_node;
using xray::editor::controls::hypergraph::node;

void property_connection_editor::execute(node^ source, node^ destination)
{
	scene_graph_node^ src	= safe_cast<scene_graph_node^>(source);
	scene_graph_node^ dst	= safe_cast<scene_graph_node^>(destination);
	
	set_src_object			(src->object);
	set_dst_object			(dst->object);
	
	if(!Visible)
		Show();
}
/*
bool is_readonly(property_container::PropertySpec^ spec)
{
	u32 att_count							= spec->Attributes->Length;
	for(u32 i=0; i<att_count; ++i)
	{
		if(spec->Attributes[i]->TypeId==ReadOnlyAttribute::typeid)
		{
			ReadOnlyAttribute^ att	= dynamic_cast<ReadOnlyAttribute^>(spec->Attributes[i]);
			return					att->IsReadOnly;
		}
	}
	return false;
}*/

System::String^ get_property_name(System::Reflection::PropertyInfo^ pi)
{
	array<System::Object^>^			attribs;
	attribs							= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
	DisplayNameAttribute^ att_name	= safe_cast<DisplayNameAttribute^>(attribs[0]);
	return							att_name->DisplayName;
}

void property_connection_editor::in_constructor()
{
	m_project					= m_level_editor->get_project();
	m_link_storage				= m_project->get_link_storage();

	m_items_style				= gcnew cli::array<item_style^>(5){gcnew item_style, gcnew item_style, gcnew item_style, gcnew item_style, gcnew item_style};
	item_style^ itm				= m_items_style[(int)item_style_id::e_connected];
	itm->m_font					= gcnew System::Drawing::Font(left_props_view->Font, FontStyle::Italic);
	itm->m_fore_color			= System::Drawing::Color::Gray;
	itm->m_back_color			= System::Drawing::Color::White;

	itm							= m_items_style[(int)item_style_id::e_connected_current];
	itm->m_font					= gcnew System::Drawing::Font(left_props_view->Font, FontStyle::Italic);
	itm->m_fore_color			= System::Drawing::Color::White;
	itm->m_back_color			= System::Drawing::Color::Blue;

	itm							= m_items_style[(int)item_style_id::e_not_connected];
	itm->m_font					= gcnew System::Drawing::Font(left_props_view->Font, FontStyle::Regular);
	itm->m_fore_color			= System::Drawing::Color::Black;
	itm->m_back_color			= System::Drawing::Color::White;

	itm							= m_items_style[(int)item_style_id::e_connection_enabled];
	itm->m_font					= gcnew System::Drawing::Font(left_props_view->Font, FontStyle::Regular);
	itm->m_fore_color			= System::Drawing::Color::Black;
	itm->m_back_color			= System::Drawing::Color::White;

	itm							= m_items_style[(int)item_style_id::e_connection_disabled];
	itm->m_font					= gcnew System::Drawing::Font(left_props_view->Font, FontStyle::Bold);
	itm->m_fore_color			= System::Drawing::Color::Gray;
	itm->m_back_color			= System::Drawing::Color::White;
	update_buttons				();
}

void property_connection_editor::listViewMenuItem_Click(System::Object^, System::EventArgs^)
{
	left_props_view->View		= System::Windows::Forms::View::List;
	right_props_view->View		= System::Windows::Forms::View::List;
	detailsViewMenuItem->Checked = false;
}

void property_connection_editor::detailsViewMenuItem_Click(System::Object^, System::EventArgs^)
{
	left_props_view->View		= System::Windows::Forms::View::Details;
	right_props_view->View		= System::Windows::Forms::View::Details;
	listViewMenuItem->Checked	= false;
}

void property_connection_editor::set_src_object(object_base^ o)
{
	m_object_src			= o;
	if(o)
		left_label->Text	= m_object_src->get_name();
	else
		left_label->Text	= nullptr;


	fill_object_props		(left_props_view, m_object_src, false);
	colorize_items			(true);
}

void property_connection_editor::set_dst_object(object_base^ o)
{
	m_object_dst			= o;
	if(o)
		right_label->Text	= m_object_dst->get_name();
	else
		right_label->Text	= nullptr;

	fill_object_props		(right_props_view, m_object_dst, false);
	colorize_items			(false);
}

void property_connection_editor::btn_swap_Click(System::Object^, System::EventArgs^)
{
	object_base^ src		= m_object_src;
	object_base^ dst		= m_object_dst;
	set_src_object			(dst);
	set_dst_object			(src);
}

void property_connection_editor::btn_grab_left_Click(System::Object^, System::EventArgs^)
{
	object_base_list^ sl			= m_level_editor->selected_objects();
	if(sl->Count!=1)
	{
		set_src_object		(nullptr);
		left_label->Text	= "select single object";
	}else
	{
		set_src_object		(sl[0]);
	}
}

void property_connection_editor::btn_grab_right_Click(System::Object^, System::EventArgs^)
{
	object_base_list^ sl			= m_level_editor->selected_objects();
	if(sl->Count!=1)
	{
		set_dst_object		(nullptr);
		right_label->Text	= "select single object";
	}else
	{
		set_dst_object		(sl[0]);
	}
}

void property_connection_editor::fill_object_props(System::Windows::Forms::ListView^ view, object_base^ o, bool)
{
	view->Items->Clear							();
	if(o==nullptr)								return;

	//::property_holder* holder					= dynamic_cast<::property_holder*>(o->get_property_holder());
	//property_container^ container				= holder->container();
	//property_container::ArrayList% properties = container->ordered_properties();

	//for each (property_container::PropertySpec^ spec in properties)
	//{
	//	System::String^ property_name			= spec->Name;
	//	System::String^ type_name				= spec->TypeId->ToString();

	//	ListViewItem^  li	= (gcnew ListViewItem(gcnew cli::array< System::String^  >(2) {property_name, type_name}, -1));
	//	li->Tag				= spec;
	//	view->Items->Add	(li);
	//}

	System::Type^ t					= o->GetType();
	array<System::Reflection::PropertyInfo^>^	type_props;
	type_props						= t->GetProperties();
	
	System::String^					property_name;
	System::Type^					property_type;
	array<System::Object^>^			attribs;
	for each (System::Reflection::PropertyInfo^ pi in type_props)
	{
		attribs							= pi->GetCustomAttributes(ConnectionAttribute::typeid, true);
		if(attribs->Length==0)
			continue;

		attribs							= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
		DisplayNameAttribute^ att_name	= safe_cast<DisplayNameAttribute^>(attribs[0]);
		property_name					= att_name->DisplayName;
		property_type					= pi->PropertyType;

		ListViewItem^  li	= (gcnew ListViewItem(gcnew cli::array< System::String^  >(2) {property_name, property_type->ToString()}, -1));
		li->Tag				= pi;
		view->Items->Add	(li);

	}

}

void property_connection_editor::colorize_items(bool b_left)
{
	item_style^ itm_style							= nullptr;

	if(b_left)
	{
		for each (ListViewItem^ li in left_props_view->Items)
		{
			if(m_link_storage->get_links(m_object_src, li->Text, nullptr, nullptr)->Count!=0)
			{
				itm_style = m_items_style[(int)item_style_id::e_connected];
			}else
			{
				itm_style = m_items_style[(int)item_style_id::e_connection_enabled];
			}
			li->Font		= itm_style->m_font;
			li->ForeColor	= itm_style->m_fore_color;
			li->BackColor	= itm_style->m_back_color;
		}
	}
	System::Reflection::PropertyInfo^ selected_pi = nullptr;
	//property_container::PropertySpec^ selected_spec = nullptr;
	System::Type^ selected_type						= nullptr;
	if(left_props_view->SelectedItems->Count!=0)
	{
		selected_pi	= (System::Reflection::PropertyInfo^)left_props_view->SelectedItems[0]->Tag;
		//selected_spec	= (property_container::PropertySpec^)left_props_view->SelectedItems[0]->Tag;
		selected_type	= selected_pi->PropertyType;
	}

	for each (ListViewItem^ li in right_props_view->Items)
	{
		System::Reflection::PropertyInfo^ current_pi = (System::Reflection::PropertyInfo^)li->Tag;
		//property_container::PropertySpec^ current_spec = (property_container::PropertySpec^)li->Tag;

		//if(is_readonly(current_spec))
		//{
		//	itm_style = m_items_style[(int)item_style_id::e_connection_disabled];
		//}else
		if(selected_pi && m_link_storage->get_links(m_object_src, get_property_name(selected_pi), m_object_dst, li->Text)->Count!=0)
		{
			itm_style = m_items_style[(int)item_style_id::e_connected_current];
		}else
		if(m_link_storage->get_links(nullptr, nullptr, m_object_dst, li->Text)->Count!=0)
		{
			itm_style = m_items_style[(int)item_style_id::e_connected];
		}else
		{
			if(selected_pi)
			{
				if(current_pi->PropertyType==selected_type)
				{
					itm_style = m_items_style[(int)item_style_id::e_connection_enabled];
				}else
				{
					itm_style = m_items_style[(int)item_style_id::e_connection_disabled];
				}
			}else
			{
				itm_style = m_items_style[(int)item_style_id::e_connection_enabled];
			}
		}
		li->Font		= itm_style->m_font;
		li->ForeColor	= itm_style->m_fore_color;
		li->BackColor	= itm_style->m_back_color;
	}
}

void property_connection_editor::update_buttons()
{
	make_connection_button->Enabled	= false;
	disconnect_button->Enabled		= false;

	if(left_props_view->SelectedItems->Count!=0 && right_props_view->SelectedItems->Count!=0)
	{
		//property_container::PropertySpec^ left_spec = (property_container::PropertySpec^)left_props_view->SelectedItems[0]->Tag;
		//property_container::PropertySpec^ right_spec = (property_container::PropertySpec^)right_props_view->SelectedItems[0]->Tag;
		System::Reflection::PropertyInfo^ left_pi	= (System::Reflection::PropertyInfo^)left_props_view->SelectedItems[0]->Tag;
		System::Reflection::PropertyInfo^ right_pi	= (System::Reflection::PropertyInfo^)right_props_view->SelectedItems[0]->Tag;
		if(left_pi->PropertyType==right_pi->PropertyType)
		{
			if(m_link_storage->get_links(m_object_src, get_property_name(left_pi), m_object_dst, get_property_name(right_pi))->Count!=0)
			{
				make_connection_button->Enabled	= false;
				disconnect_button->Enabled		= true;
			}else
			{
				make_connection_button->Enabled	= true;
				disconnect_button->Enabled		= false;
			}
		}
	}
}

void property_connection_editor::left_props_view_SelectedIndexChanged(System::Object^, System::EventArgs^)
{
	for each (ListViewItem^ itm in right_props_view->SelectedItems)
		itm->Selected = false;

	colorize_items						(false);
	update_buttons						();
}

void property_connection_editor::close_button_Click(System::Object^, System::EventArgs^)
{
	Hide		();
}

void property_connection_editor::property_connection_editor_FormClosing(System::Object^, System::Windows::Forms::FormClosingEventArgs^  e)
{
	e->Cancel	= true;
	Hide		();
}

void property_connection_editor::disconnect_button_Click(System::Object^, System::EventArgs^)
{
	//property_container::PropertySpec^ left_spec = (property_container::PropertySpec^)left_props_view->SelectedItems[0]->Tag;
	//property_container::PropertySpec^ right_spec = (property_container::PropertySpec^)right_props_view->SelectedItems[0]->Tag;
	System::Reflection::PropertyInfo^ left_pi	= (System::Reflection::PropertyInfo^)left_props_view->SelectedItems[0]->Tag;
	System::Reflection::PropertyInfo^ right_pi	= (System::Reflection::PropertyInfo^)right_props_view->SelectedItems[0]->Tag;

	//m_link_storage->remove_link	(m_object_src, left_spec->Name, m_object_dst, right_spec->Name);

	xray::editor_base::command_engine^	command_engine = m_level_editor->get_command_engine();

	command_engine->run( gcnew command_create_remove_link (
		enum_action::enum_link_action_remove,
		m_link_storage, 
		m_object_src, 
		get_property_name(left_pi), 
		m_object_dst, 
		get_property_name(right_pi))); 

	colorize_items						(true);
	update_buttons						();
}

void property_connection_editor::make_connection_button_Click(System::Object^, System::EventArgs^)
{
	//property_container::PropertySpec^ left_spec = (property_container::PropertySpec^)left_props_view->SelectedItems[0]->Tag;
	//property_container::PropertySpec^ right_spec = (property_container::PropertySpec^)right_props_view->SelectedItems[0]->Tag;
	System::Reflection::PropertyInfo^ left_pi	= (System::Reflection::PropertyInfo^)left_props_view->SelectedItems[0]->Tag;
	System::Reflection::PropertyInfo^ right_pi	= (System::Reflection::PropertyInfo^)right_props_view->SelectedItems[0]->Tag;

	xray::editor_base::command_engine^	command_engine = m_level_editor->get_command_engine();
	
	command_engine->run( gcnew command_create_remove_link (
											enum_action::enum_link_action_create,
											m_link_storage, 
											m_object_src, 
											get_property_name(left_pi), 
											m_object_dst, 
											get_property_name(right_pi))); 

	colorize_items						(true);
	update_buttons						();
}

void property_connection_editor::right_props_view_ItemSelectionChanged(System::Object^, System::Windows::Forms::ListViewItemSelectionChangedEventArgs^)
{
	update_buttons();
}

void property_connection_editor::right_props_view_DrawItem(System::Object^, System::Windows::Forms::DrawListViewItemEventArgs^  e)
{
	e->DrawBackground();
	e->DrawText();

	System::Drawing::Pen^ pen = gcnew System::Drawing::Pen(System::Drawing::Color::Red);
	e->Graphics->DrawRectangle(pen, e->Bounds);
}

void  property_connection_editor::right_props_view_DrawColumnHeader(System::Object^, System::Windows::Forms::DrawListViewColumnHeaderEventArgs^  e)
{
	e->DrawBackground();
	e->DrawText();
}

void property_connection_editor::right_props_view_DrawSubItem(System::Object^, System::Windows::Forms::DrawListViewSubItemEventArgs^  e)
{
	e->DrawBackground();
	e->DrawText();
	e->DrawFocusRectangle(e->Bounds);
}
