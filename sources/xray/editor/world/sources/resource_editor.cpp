////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_editor.h"
#include "editor_world.h"
#include "resource_editor_commit.h"
#include "resource_editor_cancel.h"
#include "tool_window_storage.h"

using namespace System::Windows;
using xray::editor::wpf_controls::property_grid::value_changed_event_handler;
using xray::resources::unmanaged_resource_ptr;

namespace xray {
namespace editor {


void resource_editor::in_constructor( )
{
	m_holder									= m_editor_world.m_tool_windows;

	m_need_view_selected						= false;
	m_need_refresh_properties					= false;
	m_need_sort_tree							= false;
	m_need_reset_grid_on_revert					= true;
	m_multidocument								= gcnew controls::document_editor_base("resource_editor", false);

	m_multidocument->content_reloading			= gcnew controls::content_reload_callback(this, &resource_editor::layout_for_panel_callback);
	m_multidocument->creating_new_document		= gcnew controls::document_create_callback(this, &resource_editor::on_document_creating);

	m_properties_panel							= gcnew controls::item_properties_panel_base(m_multidocument);
	m_properties_panel->property_grid_control->property_value_changed	+= gcnew value_changed_event_handler(this, &resource_editor::on_property_value_changed);

	m_images									= m_editor_world.images16x16();
	
	m_opened_resources							= gcnew resources_dict;
	m_changed_resources							= gcnew resources_list;
	m_selected_resources						= gcnew resources_list;

	if(is_selector)
	{
		m_ok_button->Text = "OK";

		m_multidocument->is_fixed_panels		= true;
		m_multidocument->is_single_document		= true;
		m_multidocument->remove_context_menu();
	}

	m_multidocument->Dock	= DockStyle::Fill;
	Controls->Add			(m_multidocument);
	Controls->SetChildIndex	(m_multidocument, 0);

	if(is_selector)
		this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;

	m_holder->register_tool_window( this );
}

void resource_editor::on_menu_opening( System::Object^ , System::ComponentModel::CancelEventArgs^ )
{

}

void resource_editor::in_destructor( )
{
	m_holder->unregister_tool_window( this );
}

void resource_editor::tick()
{
	if(m_need_sort_tree)
	{
		resources_tree_view->Sort	( );
		m_need_sort_tree			= false;
	}
}

void resource_editor::resource_editor_Load( Object^ ,System::EventArgs^ )
{
	resources_tree_view->items_loaded			+= gcnew System::EventHandler(this, &resource_editor::on_resource_list_loaded);
	resources_tree_view->ContextMenuStrip->Items->Add( revert_toolStripMenuItem );
	resources_tree_view->ContextMenuStrip->Items->Add( applyToolStripMenuItem );
	resources_tree_view->ContextMenuStrip->Items->Add( expandAllToolStripMenuItem );
	resources_tree_view->ContextMenuStrip->Items->Add( collapseToolStripMenuItem );
	resources_tree_view->ContextMenuStrip->Opening += gcnew System::ComponentModel::CancelEventHandler( this, &resource_editor::on_menu_opening );

	revert_toolStripMenuItem->Click				+= gcnew System::EventHandler(this, &resource_editor::revertToolStripMenuItem_Click);
	applyToolStripMenuItem->Click				+= gcnew System::EventHandler(this, &resource_editor::applyToolStripMenuItem_Click);
	expandAllToolStripMenuItem->Click			+= gcnew System::EventHandler(this, &resource_editor::expandAllToolStripMenuItem_Click);
	collapseToolStripMenuItem->Click			+= gcnew System::EventHandler(this, &resource_editor::collapseAllToolStripMenuItem_Click);



	if(is_selector)
	{
		resources_tree_view->AfterSelect			+= gcnew TreeViewEventHandler(this, &resource_editor::tree_view_selection_changed);
		resources_tree_view->NodeMouseDoubleClick	+= gcnew TreeNodeMouseClickEventHandler(this, &resource_editor::tree_view_node_double_click);
		if(resources_tree_view->selected_nodes->Count == 0)
			m_ok_button->Enabled = false;
	}

	load_panels_layout();  //Calls load panels from nested type
}

IDockContent^ resource_editor::layout_for_panel_callback( System::String^ panel_name )
{
	return layout_for_panel(panel_name);
}

controls::document_base^ resource_editor::on_document_creating( )
{
	return create_new_document();
}

void resource_editor::on_resource_list_loaded( Object^ ,System::EventArgs^ )
{
	if(is_selector)
	{
		m_is_resource_list_loaded = true;
		if(m_track_resource_name != nullptr)
			resources_tree_view->track_active_node(m_track_resource_name);
	}else
	{
		RegistryKey^ editor_key = editor_registry_key();

		System::String^ selected_path			= safe_cast<System::String^>(editor_key->GetValue("tree_view_selected_path"));
		if(selected_path != nullptr && selected_path != System::String::Empty)
			resources_tree_view->track_active_node(selected_path);
	}
}

void resource_editor::tree_view_selection_changed( Object^ , TreeViewEventArgs^ )
{
	if(resources_tree_view->selected_nodes->Count == 0)
		m_ok_button->Enabled = false;
	else
		m_ok_button->Enabled = true;
}

void resource_editor::tree_view_node_double_click( Object^ sender, TreeNodeMouseClickEventArgs^ e)
{
	if(safe_cast<controls::tree_node^>(e->Node)->m_node_type == controls::tree_node_type::single_item)
	{
		m_ok_button_Click(sender, e);
	}
}

void resource_editor::on_property_value_changed( Object^ , value_changed_event_args^)
{
	for each(resource_editor_resource^ wrapper in m_selected_resources)
		make_wrapper_modified	( wrapper );
	
	m_properties_panel->property_grid_control->update();
	
	on_resource_changed							( );
	
	if (m_is_apply_resource_on_property_value_changed)
	{
		resources_list							selected_copy;
		selected_copy.AddRange					( m_selected_resources );
		for each ( resource_editor_resource^ r in selected_copy )
		{
			/*controls::tree_node^ node			= */resources_tree_view->get_node( r->m_name );
			//remove_modify_status_recurcively	( node, resource_image_index, modified_resource_image_index );
			//m_changed_resources->Remove			( r );
			r->preview_changes					( );
		}
	}
}

static void remove_modify_status_recurcively(controls::tree_node^ node, int resource_image_index, int modified_resource_image_index)
{
	while(node != nullptr)
	{
		if(node->m_node_type == controls::tree_node_type::single_item)
		{
			node->m_image_index_collapsed			= resource_image_index;
			node->m_image_index_expanded			= resource_image_index;
			node->ImageIndex						= resource_image_index; 
			node->SelectedImageIndex				= resource_image_index; 
		}else
		{
			for each(controls::tree_node^ n in node->nodes)
			{
				if(	n->ImageIndex == modified_resource_image_index)
					return;
			}

			node->m_image_index_collapsed			= xray::editor_base::folder_closed;
			node->m_image_index_expanded			= xray::editor_base::folder_open;
			node->ImageIndex						= xray::editor_base::folder_open;
			node->SelectedImageIndex				= xray::editor_base::folder_open;
		}
		node = node->Parent;
	}
}

void resource_editor::revert_properties( resource_editor_resource^ resource )
{
	if( resource->m_is_new )
	{
		System::String^ full_name		= resource->m_name;
		m_opened_resources->Remove		( full_name );

		controls::tree_node^ node		= resources_tree_view->get_node( full_name );
		resources_tree_view->nodes->Remove( node );

		if( m_changed_resources->Contains( resource ) )
			m_changed_resources->Remove( resource );

		if( m_selected_resources->Contains( resource ) )
			m_selected_resources->Remove( resource );

		delete		resource;
		return;
	}
	
	if( resource->m_is_renamed )
	{
		System::String^ full_name		= resource->m_name;
		System::String^ old_full_name	= resource->m_old_name;
		m_opened_resources->Remove		( full_name );

		m_opened_resources->Add			( old_full_name, resource );

		controls::tree_node^ node		= resources_tree_view->get_node( full_name );
		int idx_of_name					= old_full_name->LastIndexOf('/');
		if(idx_of_name!=-1)
			++idx_of_name;
		else
			idx_of_name = 0;

		System::String^ node_name		= old_full_name->Substring( idx_of_name );
		node->Text						= node_name;
		node->Name						= node_name;
		m_need_sort_tree				= true;
	}

	resource->reset_to_default			( );
}

void resource_editor::revert_changes( resources_list^ selected )
{
	resources_list							selected_copy;
	selected_copy.AddRange					( selected );

	for each ( resource_editor_resource^ r in selected_copy )
	{
		controls::tree_node^ node			= resources_tree_view->get_node( r->m_name );
		remove_modify_status_recurcively	( node, resource_image_index, modified_resource_image_index );

		m_changed_resources->Remove			( r );

		revert_properties					( r );
	}

//.	m_need_refresh_properties				= true;
	show_selected_options					( );
}

void resource_editor::apply_changes( resources_list^ selected )
{
	resources_list							selected_copy;
	selected_copy.AddRange					( selected );

	for each ( resource_editor_resource^ r in selected_copy )
	{
		controls::tree_node^ node			= resources_tree_view->get_node( r->m_name );
		remove_modify_status_recurcively	( node, resource_image_index, modified_resource_image_index );

		m_changed_resources->Remove			( r );

		apply_properties					( r );
	}
}

void resource_editor::revertToolStripMenuItem_Click	(Object^ ,System::EventArgs^ )
{
	revert_changes							( m_selected_resources );
}

void resource_editor::apply_properties( resource_editor_resource^ resource )
{
	resource->apply_changes						( );

	resource->save								( );

	if( resource->m_is_deleted )
	{
		System::String^ full_path			= resource->m_name;
		m_opened_resources->Remove			( full_path );

		
		if(m_changed_resources->Contains	( resource ) )
			m_changed_resources->Remove		( resource );
		
		if(m_selected_resources->Contains	( resource ) )
			m_selected_resources->Remove	( resource );

		delete resource;

		controls::tree_node^ node			= resources_tree_view->get_node( full_path );
		resources_tree_view->nodes->Remove( node );
	}
}

void resource_editor::expandAllToolStripMenuItem_Click( Object^, System::EventArgs^ )
{
	resources_tree_view->ExpandAll();
}

void resource_editor::collapseAllToolStripMenuItem_Click( Object^, System::EventArgs^ )
{
	resources_tree_view->CollapseAll();
}

void resource_editor::applyToolStripMenuItem_Click(Object^ ,System::EventArgs^ )
{
	apply_changes( m_selected_resources );// double
}

//////////////////////////////////////////////////////////////////////////
//							Protected Data								//
//////////////////////////////////////////////////////////////////////////

RegistryKey^ resource_editor::editor_registry_key( )
{
	RegistryKey^ windows			= base_registry_key::get()->CreateSubKey("windows");
	return	windows->CreateSubKey	(m_multidocument->Name);
}

void resource_editor::make_wrapper_modified( resource_editor_resource^ wrapper )
{
	controls::tree_node^ node				= resources_tree_view->get_node( wrapper->m_name );

	while(node != nullptr)
	{
		if(node->m_node_type == controls::tree_node_type::single_item)
		{
			int img = wrapper->m_is_broken ? xray::editor_base::npc_tree_icon : modified_resource_image_index;
			node->m_image_index_collapsed			= img;
			node->m_image_index_expanded			= img;
			node->ImageIndex						= img; 
			node->SelectedImageIndex				= img; 
		}else
		{
			node->m_image_index_collapsed			= editor_base::folder_closed_modified;
			node->m_image_index_expanded			= editor_base::folder_open_modified;
			node->ImageIndex						= editor_base::folder_open_modified; 
			node->SelectedImageIndex				= editor_base::folder_open_modified; 
		}
		node = node->Parent;
	}

	if( !m_changed_resources->Contains(wrapper) && !wrapper->m_is_broken )
		m_changed_resources->Add	(wrapper);
}

void resource_editor::resource_editor_FormClosing( System::Object^  , System::Windows::Forms::FormClosingEventArgs^ e )
{
	bool bcancel = this->DialogResult==Forms::DialogResult::None || this->DialogResult==Forms::DialogResult::Cancel;

	if( resolve_changed_resources( bcancel ) )
		clear_resources ( );
	else
		e->Cancel	= true;
}

bool resource_editor::resolve_changed_resources( bool cancel_action )
{
	if(m_changed_resources->Count == 0)
		return true;

	if(!cancel_action)
	{
		resource_editor_commit^	 commit_box = gcnew resource_editor_commit( this );
		Forms::DialogResult result		= commit_box->ShowDialog();

		if( result == Forms::DialogResult::OK )
		{
			apply_changes	( m_changed_resources );
			return			true;
		}else
		{
			return			false;
		}
	}else
	{// cancel action
		names_list^ changed_names = gcnew names_list;
		
		for each ( resource_editor_resource^ r in m_changed_resources)
			changed_names->Add( r->m_name );

		resource_editor_cancel^ cancel_dlg	= gcnew resource_editor_cancel(changed_names);
		bool cancel							= (cancel_dlg->ShowDialog() == Forms::DialogResult::OK);

		if(!cancel)
		{
			return false;
		}else
		{
			revert_changes	( m_changed_resources );
			return			true;
		}
	}

}

void resource_editor::close_internal( Forms::DialogResult result )
{
	this->DialogResult	= result;

	if(false == this->Modal)
	{
		save_settings		( base_registry_key::get() );
		Close				( );
	}
}

void resource_editor::m_ok_button_Click( Object^ ,System::EventArgs^ )
{
	if(!resolve_changed_resources(false))
		return;

	if(is_selector)
	{
		if(m_multidocument->active_document != nullptr)
			m_multidocument->active_document->m_need_suppress_loading = true;
		
		if(selecting_entity == entity::resource_name)
		{
			close_internal		( Forms::DialogResult::OK );
		}else
		if(selecting_entity == entity::resource_object)
		{
			if(m_resources_loaded)
			{
				close_internal		( Forms::DialogResult::OK );
			}else
			{
				m_commit_on_loaded		= true;
				m_ok_button->Text		= "Loading...";
				m_ok_button->Enabled	= false;
			}
		}
	}else
	{
		close_internal			( Forms::DialogResult::OK );
	}
}

void resource_editor::m_cancel_button_Click( Object^ ,System::EventArgs^ )
{
	if(!resolve_changed_resources(true))
		return;

	close_internal			( Forms::DialogResult::Cancel );
}

void resource_editor::resources_loaded( Object^ ,System::EventArgs^ )
{
	m_resources_loaded	= true;

	if(m_commit_on_loaded)
		close_internal		( Forms::DialogResult::OK );
}

bool resource_editor::load_panels_layout( )
{
	bool is_loaded_normally = m_multidocument->load_panels		(this);

	if(is_loaded_normally)
		m_properties_panel->Show	();
	else
		m_properties_panel->Show	(m_multidocument->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockRight);
	
	return is_loaded_normally;
}

IDockContent^ resource_editor::layout_for_panel( System::String^ panel_name )
{
	if(panel_name == "xray.editor.controls.item_properties_panel_base")		
		return m_properties_panel;
	
	R_ASSERT("there is no panel with name: "+panel_name);
	return nullptr;
}

void resource_editor::raise_property_value_changed( )
{
	on_property_value_changed	( this, nullptr );
}

bool resource_editor::close_query( )
{
	return (m_changed_resources->Count == 0);
}

bool resource_editor::is_resource_loaded( System::String^ resource_name )
{
	return m_opened_resources->ContainsKey(resource_name);
}

resource^ resource_editor::get_loaded_resource( System::String^ resource_name )
{
	return m_opened_resources[resource_name];
}

void resource_editor::clear_resources( )
{
	RegistryKey^ editor_key = editor_registry_key();
	if( resources_tree_view->selected_nodes->Count > 0 && resources_tree_view->selected_nodes[0]->TreeView != nullptr)
			editor_key->SetValue(
			"tree_view_selected_path",
			resources_tree_view->selected_nodes[0]->FullPath
			);

	m_multidocument->close_all_documents( );
	m_multidocument->save_panels		( this );

	for each(resource_editor_resource^ resource in m_opened_resources->Values)
		delete resource;
}

void resource_editor::Show( System::String^ context1, System::String^ context2 )
{
	XRAY_UNREFERENCED_PARAMETER( context1 );
	System::Windows::Forms::Form::Show();
	System::Windows::Forms::Form::Focus();

	if( context2!= nullptr )
		selected_name = context2;
}

void resource_editor::view_selected_options_impl( )
{
	m_properties_panel->property_grid_control->Enabled = false;

	if(m_selected_resources->Count!=0)
	{
		array<Object^>^ objects = gcnew array<Object^>(m_selected_resources->Count);
	
		for (int i=0; i<m_selected_resources->Count; ++i )
		{
			resource_editor_resource^ r = m_selected_resources[i];
			if( !r->loaded )
				return;

			objects[i]					= r->get_property_container( is_selector );
		}
				
		m_properties_panel->property_grid_control->selected_objects = objects;
	}else
	{
		m_properties_panel->property_grid_control->selected_object = nullptr;
	}

	m_need_refresh_properties							= false;
	m_properties_panel->property_grid_control->Enabled	= true;
}

}//namespace editor
}//namespace xray