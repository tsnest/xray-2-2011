////////////////////////////////////////////////////////////////////////////
//	Created		: 20.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "library_object_tab.h"
#include "tool_base.h"
#include "object_base.h"
#include "property_holder.h"
#include "property_container.h"
#include "level_editor.h"
#include "editor_world.h"

using xray::editor::library_object_tab;
using xray::editor::tool_base;
using xray::editor::object_base;
using xray::editor::controls::property_grid;

void library_object_tab::in_constructor()
{
	m_current_mode				= mode_none;
	m_property_grid				= gcnew property_grid();
	m_property_grid->Dock		= System::Windows::Forms::DockStyle::Fill;

	SuspendLayout				();
	bottom_panel->Controls->Add	(m_property_grid);

	m_property_grid_restrictions		= gcnew property_grid();
	m_property_grid_restrictions->Dock	= System::Windows::Forms::DockStyle::Fill;

	tabPage2->Controls->Add		(m_property_grid_restrictions);
	ResumeLayout				(false);

	m_library_object_ph			= NEW(::property_holder)("lib_objs", NULL, NULL);
}

void library_object_tab::in_destructor()
{
	property_holder* ph			= m_library_object_ph;
	DELETE						(ph);
}

void library_object_tab::create_new(tool_base^ tool)
{
	m_current_mode							= mode_new;
	clear									();
	
	m_tool									= tool;
	m_library_item							= m_tool->new_lib_item("new_library_item");
	m_tool->get_objects_list				(raw_objects_selector_cb->Items, lm_raw_objects);
	raw_objects_selector_cb->Enabled		= true;
	raw_objects_selector_cb->SelectedIndex	= 0;
}

void library_object_tab::edit_existing(tool_base^ tool, System::String^ name)
{
	m_current_mode							= mode_edit;
	clear									();

	m_tool									= tool;
	m_library_item							= m_tool->get_library_item(name, true);
	raw_objects_selector_cb->Items->Add		(gcnew System::String(name));
	raw_objects_selector_cb->Enabled		= false;
	raw_objects_selector_cb->SelectedIndex	= 0;
}

System::Void library_object_tab::raw_objects_selector_cb_TextChanged(System::Object^  sender, System::EventArgs^  e)
{
	XRAY_UNREFERENCED_PARAMETERS			( sender, e );

	destroy_current_object					();
	create_current_object					();
	
	m_current_object->set_selected			(true);

	//reset restrictions
	::property_holder*		properties		= dynamic_cast< ::property_holder* >(m_current_object->get_property_holder() );

	m_library_item->m_property_restrictor->create_from	(properties);
	m_property_grid->SelectedObject			= properties->container	();


	::property_holder*		properties_restr = dynamic_cast< ::property_holder* >(m_library_item->m_property_restrictor->m_property_holder );

	m_property_grid_restrictions->SelectedObject = properties_restr->container	();
}

void library_object_tab::clear()
{
	raw_objects_selector_cb->Items->Clear			();
	m_property_grid->SelectedObject					= nullptr;
	m_property_grid_restrictions->SelectedObject	= nullptr;
	destroy_current_object							();
}

void library_object_tab::on_render()
{
	if(m_library_item && m_current_object)
	{
 		float4x4 transform;
 		transform.identity();
 		transform.c.y = 15.0f;
		m_current_object->render();
	}
}

void library_object_tab::create_current_object()
{
	System::String^ s		= raw_objects_selector_cb->Text;
	if(raw_objects_selector_cb->Text->StartsWith("base:"))
	{
		m_current_object = m_tool->create_raw_object(s);
		m_current_object->set_lib_name(s);
	}else
	{
		m_current_object = m_tool->create_object(s);
		lib_item^ li = m_tool->get_library_item(s, true);
		m_current_object->set_lib_name(li->get_raw_lib_name());
	}

	if(m_current_mode == mode_new)
		m_current_object->set_name("New", true);

	m_current_object->assign_id		(0);
	m_current_object->load_defaults	();
}

void library_object_tab::destroy_current_object()
{
	if(m_current_object)
	{
		m_tool->destroy_object(m_current_object);
		m_current_object = nullptr;
	}
}

// menu handlers
System::Void library_object_tab::on_commit(System::Object^, System::EventArgs^)
{
	m_property_grid->SelectedObject	= nullptr;

	m_library_item->m_lib_item_name	= m_current_object->get_name();
	m_current_object->save			(*m_library_item->m_config);

	m_library_item->m_property_restrictor->save(*m_library_item->m_config);

	m_tool->on_commit_library_item	(m_library_item);
	m_tool->save_library();
	clear							();
	Hide							();
}

System::Void library_object_tab::on_discard(System::Object^, System::EventArgs^)
{
	clear							();
	Hide							();
}

