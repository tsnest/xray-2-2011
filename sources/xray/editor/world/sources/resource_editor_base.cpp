////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_editor_base.h"
#include "texture_editor_source.h"
#include "texture_document.h"
#include "resource_document_factory.h"
#include "images/images16x16.h"
#include <xray/editor/base/images_loading.h>

using WeifenLuo::WinFormsUI::Docking::DockContent;
using WeifenLuo::WinFormsUI::Docking::DockState;

using xray::editor::controls::document_base;
using Flobbster::Windows::Forms::PropertySpec;

using xray::editor::wpf_controls::property_grid_value_changed_event_handler;

namespace xray {
namespace editor {

void 						resource_editor_base::in_constructor				()
{
	m_images							= xray::editor_base::image_loader::load_images("images16x16", "xray.editor.resources", 16, safe_cast<int>(images16x16_count), this->GetType()->Assembly);
	
	m_opened_resources					= gcnew Collections::Generic::Dictionary<String^, resource_editor_options_wrapper^>();
	m_changed_resources					= gcnew Collections::Generic::Dictionary<String^, resource_editor_options_wrapper^>();
	m_selected_resources				= gcnew Collections::Generic::List<resource_editor_options_wrapper^>();

	need_load_panels_from_reestr		= true;
}

void						resource_editor_base::on_resource_selected			(Object^ , EventArgs^ )
{
	m_selected_resources->Clear();
	controls::tree_node^ last_node;
	for each(controls::tree_node^ node in view_panel->tree_view->selected_nodes)
	{
		if(node->m_node_type == controls::tree_node_type::single_item)
		{
			if(is_resource_loaded(node->FullPath))
				m_selected_resources->Add(get_loaded_resource(node->FullPath));
		}
		last_node = node;
	}
	if( last_node != nullptr )
		load_document					(last_node->FullPath);

	bool can_revert_or_apply = false;

	for each(TreeNode^ node in view_panel->tree_view->selected_nodes)
	{
		if(node->ImageIndex == node_resource_modified)
		{
			can_revert_or_apply = true;
			break;
		}
	}

	revert_toolStripMenuItem->Enabled	= can_revert_or_apply;
	applyToolStripMenuItem->Enabled		= can_revert_or_apply;
}

void						resource_editor_base::on_property_value_changed		(Object^ , property_grid_value_changed_event_args^)
{
	Boolean has_modified_item = false;
	for each(resource_editor_options_wrapper^ wrapper in m_selected_resources)
	{
		if(!wrapper->m_is_changed)
		{	
			controls::tree_node^ node				= view_panel->tree_view->get_node(gcnew String(static_cast<resource_options*>(wrapper->m_resource->c_ptr())->m_resource_name.c_str()));

			while(node != nullptr)
			{
				if(node->m_node_type == controls::tree_node_type::single_item)
				{
					node->m_image_index_collapsed			= node_resource_modified;
					node->m_image_index_expanded			= node_resource_modified;
					node->ImageIndex						= node_resource_modified; 
					node->SelectedImageIndex				= node_resource_modified; 
				}
				else
				{
					node->m_image_index_collapsed			= folder_closed_modified;
					node->m_image_index_expanded			= folder_open_modofied;
					node->ImageIndex						= folder_open_modofied; 
					node->SelectedImageIndex				= folder_open_modofied; 
				}
				node = node->Parent;
			}
			wrapper->m_is_changed		= true;
			m_changed_resources->Add	(wrapper->m_resource_name, wrapper);
			has_modified_item			= true;
		}
	}
	if(has_modified_item)
	{
		revert_toolStripMenuItem->Enabled	= true;
		applyToolStripMenuItem->Enabled		= true;
	}
}

Boolean						resource_editor_base::load_panels					(Form^ parent, Boolean create_base_panel_objects)
{
	Boolean ret_is_load_normally = false;
	XRAY_UNREFERENCED_PARAMETER										(create_base_panel_objects);
	//parent->Text													= m_resource_base->get_editor_name();
 	view_panel														= gcnew controls::file_view_panel_base(this/*, m_resource_base->get_view_panel_name()*/);
 	properties_panel												= gcnew controls::item_properties_panel_base(this/*, m_resource_base->get_properties_panel_name()*/);
	m_main_dock_panel->DockLeftPortion								= 0.25f;
	create_toolbar_panel											= false;
	
	if(need_load_panels_from_reestr)
		ret_is_load_normally = document_editor_base::load_panels	(parent, false);

	if(!ret_is_load_normally)
	{
		view_panel->Show		(m_main_dock_panel, DockState::DockLeft);
		properties_panel->Show	(m_main_dock_panel, DockState::DockRight);
	}

	is_single_document												= true;
	view_panel->tree_view->LabelEdit								= false;
	view_panel->tree_view->ContextMenu								= nullptr;
	creating_new_document											= gcnew controls::document_create_callback(this, &resource_editor_base::on_document_creating);
	view_panel->tree_view->selected_items_changed					+= gcnew EventHandler(this, &resource_editor_base::on_resource_selected);
	properties_panel->property_grid_control->property_value_changed	+= gcnew property_grid_value_changed_event_handler(this, &resource_editor_base::on_property_value_changed);
	view_panel->tree_view->items_loaded								+= gcnew EventHandler(this, &resource_editor_base::on_item_list_loaded);
	view_panel->tree_view->ImageList								= m_images;
	view_panel->tree_view->nodes_context_menu						= m_revertContextMenuStrip;
	view_panel->tree_view->source									= m_resources_source;
	view_panel->tree_view->is_selectable_groups						= true;
	view_panel->tree_view->refresh();
	view_panel->tree_view->is_multiselect							= true;

	return ret_is_load_normally;
}

controls::document_base^	resource_editor_base::on_document_creating			()
{
	return m_document_factory->new_document(this);
}

void						resource_editor_base::on_item_list_loaded			(Object^ , EventArgs^ )
{
	resource_list_loaded(this, EventArgs::Empty);
}

void						resource_editor_base::set_options_and_holder		(const resources::unmanaged_resource_ptr& options_ptr, xray::editor_base::property_holder* holder)
{
	String^ key = gcnew String(dynamic_cast<resource_options*>(options_ptr.c_ptr())->m_resource_name.c_str());
	resource_editor_options_wrapper^ wrapper = gcnew resource_editor_options_wrapper(m_editor_world, options_ptr, holder);
	m_opened_resources->Add(key, wrapper);
}

void						resource_editor_base::free_resources				()
{
	for each(resource_editor_options_wrapper^ wrapper in m_opened_resources->Values)
	{
		wrapper->~resource_editor_options_wrapper();
	}
	resource_editor_closing(this, EventArgs::Empty);
}

Boolean						resource_editor_base::is_resource_loaded			(String^ resource_name)
{
	return m_opened_resources->ContainsKey(resource_name);
}

options_wrapper^			resource_editor_base::get_loaded_resource			(String^ resource_name)
{
	return m_opened_resources[resource_name];
}

static void					remove_modify_status_recurcively(controls::tree_node^ node)
{
	while(node != nullptr)
	{
		if(node->m_node_type == controls::tree_node_type::single_item)
		{
			node->m_image_index_collapsed			= node_resource;
			node->m_image_index_expanded			= node_resource;
			node->ImageIndex						= node_resource; 
			node->SelectedImageIndex				= node_resource; 
		}
		else
		{
			for each(controls::tree_node^ n in node->nodes)
			{
				if(n->ImageIndex == node_resource_modified)
					return;
			}

			node->m_image_index_collapsed			= folder_closed;
			node->m_image_index_expanded			= folder_open;
			node->ImageIndex						= folder_open; 
			node->SelectedImageIndex				= folder_open; 
		}
		node = node->Parent;
	}
}

void						resource_editor_base::revertToolStripMenuItem_Click	(Object^ , EventArgs^ )
{
	for each(resource_editor_options_wrapper^ wrapper in m_selected_resources)
	{
		property_container^ container = wrapper->m_property_holder->container();
		ArrayList% list = container->ordered_properties();
		for each(PropertySpec^ spec in list)
		{
			container->value(spec)->set_value(spec->DefaultValue);
		}		

		controls::tree_node^ node			=	view_panel->tree_view->get_node(gcnew String(static_cast<resource_options*>(wrapper->m_resource->c_ptr())->m_resource_name.c_str()));
		remove_modify_status_recurcively		(node);

		wrapper->m_is_changed				=	false;
		m_changed_resources->Remove				(wrapper->m_resource_name);
	}

	//refresh property grid
	properties_panel->property_grid_control->Refresh();
	revert_toolStripMenuItem->Enabled	= false;
	applyToolStripMenuItem->Enabled		= false;
}

void						resource_editor_base::applyToolStripMenuItem_Click	(Object^ , EventArgs^ )
{
	for each(resource_editor_options_wrapper^ wrapper in m_selected_resources)
	{
		resource_options* options = static_cast<resource_options*>(wrapper->m_resource->c_ptr());
		options->save();
		
		property_container^ container = wrapper->m_property_holder->container();
		ArrayList% list = container->ordered_properties();
		for each(PropertySpec^ spec in list)
		{
			spec->DefaultValue = container->value(spec)->get_value();
		}	

		controls::tree_node^ node			=	view_panel->tree_view->get_node(gcnew String(options->m_resource_name.c_str()));
		remove_modify_status_recurcively		(node);

		wrapper->m_is_changed				=	false;
		m_changed_resources->Remove				(wrapper->m_resource_name);
	}
	revert_toolStripMenuItem->Enabled	= false;
	applyToolStripMenuItem->Enabled		= false;
}

} // namespace editor
} // namespace xray
