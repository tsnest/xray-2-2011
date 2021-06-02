////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_selector.h"
#include "resource_editor_base.h"
#include "texture_document_factory.h"
#include "resource_editors_factory.h"

using namespace System::Windows;

namespace xray {
namespace editor {

void				resource_selector::in_constructor							(editor_world& world, Object^ filter)
{		
	m_multidocument = gcnew resource_editor_base(world, resource_editors_factory::get_document_factory(m_resource_type), resource_editors_factory::get_resources_source(m_resource_type, filter), false);
	m_multidocument->is_fixed_panels	= true;
	m_multidocument->is_single_document	= true;
	m_multidocument->remove_context_menu();
	m_multidocument->Dock				= DockStyle::Fill;
	m_multidocument->need_load_panels_from_reestr = true;
	m_multidocument->resource_list_loaded +=
		gcnew EventHandler(this, &resource_selector::resources_list_loaded);
	m_multidocument->document_created +=
		gcnew EventHandler<controls::document_event_args^>(this, &resource_selector::document_created);
	Controls->Add						(m_multidocument);
	Controls->SetChildIndex				(m_multidocument, 0);
}

String^				resource_selector::selected_name::get						()
{
	if(selecting_entity == entity::resource_name)
		return m_multidocument->view_panel->tree_view->selected_nodes[0]->FullPath;
	
	R_ASSERT("trying to receive resource name when requesting resource");
	return String::Empty;
}

void				resource_selector::selected_name::set						(String^ value)
{
	if(m_is_resource_list_loaded)
		m_multidocument->view_panel->tree_view->track_active_node(value);
	else
	{
		m_track_resource_name = value;
	}
}

resources::unmanaged_resource_ptr	resource_selector::selected_resource::get					()
{
	if(selecting_entity == entity::resource_object)
	{
		return m_multidocument->selected_resources[0]->m_resource->c_ptr();
	}
	
	R_ASSERT("trying to receive resource when requesting resource name");
	return NULL;
}

void				resource_selector::resources_list_loaded					(Object^ , EventArgs^ )
{
	m_is_resource_list_loaded = true;
	if(m_track_resource_name != nullptr)
		m_multidocument->view_panel->tree_view->track_active_node(m_track_resource_name);
}

void				resource_selector::resources_loaded							(Object^ , EventArgs^ )
{
	m_resources_loaded = true;
	if(m_commit_on_loaded)
	{
		this->DialogResult = Forms::DialogResult::OK;
	}
}

void				resource_selector::resource_selector_Load					(System::Object^  , System::EventArgs^  )
{
	m_multidocument->load_panels		(this);
	m_multidocument->main_dock_panel->DockRightPortion = 0.4f;
	m_multidocument->view_panel->tree_view->NodeMouseDoubleClick += gcnew TreeNodeMouseClickEventHandler(this, &resource_selector::tree_view_node_double_click);
	m_multidocument->view_panel->tree_view->AfterSelect += gcnew TreeViewEventHandler(this, &resource_selector::tree_view_selection_changed);
	if(m_multidocument->view_panel->tree_view->selected_nodes->Count == 0)
		m_ok_button->Enabled = false;
}

void				resource_selector::m_ok_button_Click						(System::Object^  , System::EventArgs^  )
{
	m_multidocument->active_document->m_need_suppress_loading = true;
	if(selecting_entity == entity::resource_name)
		this->DialogResult = Forms::DialogResult::OK;
	if(selecting_entity == entity::resource_object)
	{
		if(m_resources_loaded)
			this->DialogResult		= Forms::DialogResult::OK;
		else
		{
			m_commit_on_loaded		= true;
			m_ok_button->Text		= "Loading...";
			m_ok_button->Enabled	= false;
		}
	}
}

void				resource_selector::tree_view_node_double_click		(Object^ sender, TreeNodeMouseClickEventArgs^ e)
{
	m_ok_button_Click(sender, e);
}

void				resource_selector::tree_view_selection_changed		(Object^ , TreeViewEventArgs^ )
{
	if(m_multidocument->view_panel->tree_view->selected_nodes->Count == 0)
		m_ok_button->Enabled = false;
	else
		m_ok_button->Enabled = true;
}

void				resource_selector::document_created							(Object^ , controls::document_event_args^ e)
{
	safe_cast<texture_document^>(e->document)->is_use_by_selector	= true;
	safe_cast<texture_document^>(e->document)->resource_loaded		+= gcnew EventHandler(this, &resource_selector::resources_loaded);
}

void				resource_selector::resource_selector_FormClosing			(Object^ , FormClosingEventArgs^ )
{
	m_multidocument->close_all_documents();
}

}//namespace editor
}//namespace xray