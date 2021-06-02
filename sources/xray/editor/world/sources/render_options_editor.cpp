////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_world.h"
#include "render_options_editor.h"
#include "resource_editor_resource.h"
#include "render_options_editor_resource.h"

#include "project_items.h"

#pragma managed( push, off )
#	include <xray/resources.h>
#	include <xray/resources_query_result.h>
#	include <xray/render/facade/scene_renderer.h>
#	include <xray/render/engine/base_classes.h>
#	include <xray/render/facade/editor_renderer.h>
#	include <xray/strings_stream.h>
#pragma managed( pop )


using WeifenLuo::WinFormsUI::Docking::DockState;
using namespace xray::editor::wpf_controls::property_grid;
using namespace System;
using namespace System::Collections;
using namespace System::Collections::ObjectModel;

using System::Windows::Forms::ContextMenuStrip;;
using xray::editor::wpf_controls::property_grid::value_changed_event_handler;



namespace xray{
	namespace editor{
		
		render_options_editor::render_options_editor( editor_world& world ):
			resource_editor(world, false), 
			m_render_options_source(gcnew render_options_editor_source())
		{
			m_editor_renderer		= &m_editor_world.engine().get_renderer_world().editor_renderer( );
			Name					= "render_options_editor";
			Text					= "Render Options Editor";
			in_constructor			( );
		}

		render_options_editor::~render_options_editor( )
		{
			
		}
		void render_options_editor::tick( )
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
		
		void render_options_editor::form_closed( Object^ , FormClosedEventArgs^ )
		{
			
		}
		void render_options_editor::on_resource_changed( )
		{
			
		}
		void render_options_editor::on_resource_loaded( resource_editor_resource^ resource, bool /*is_correct*/ )
		{
			if(m_selected_resources->Contains(resource))
			{
				m_need_view_selected			= true;
				for each( resource_editor_resource^ res in m_selected_resources)
					if(!res->loaded)
						m_need_view_selected	= false;
			}
		}
		void render_options_editor::on_resource_changed( resource_editor_resource^ data )
		{
			make_wrapper_modified( data );
			on_resource_loaded	 ( data, true );
		}
		void render_options_editor::on_resource_selected( Object^, controls::tree_view_selection_event_args^ )
		{
			m_selected_resources->Clear	( );
			controls::tree_node^		last_node;
			
			m_need_view_selected		= true;
			
			for each(controls::tree_node^ node in render_options_panel->tree_view->selected_nodes)
			{
				if(node->m_node_type == controls::tree_node_type::single_item)
				{
					System::String^ node_path	= node->FullPath;

					resource_editor_resource^ res		= nullptr;
					if( m_opened_resources->ContainsKey(node_path) )
						res = m_opened_resources[node_path];

					if(!res)
					{
						res		= gcnew render_options_editor_resource( 
											m_editor_world,
											node_path, 
											gcnew resource_changed_delegate(this, &render_options_editor::on_resource_changed),
											gcnew resource_load_delegate(this, &render_options_editor::on_resource_loaded) );
						m_opened_resources->Add( res->m_name, res );
					}

					m_selected_resources->Add(res);
					last_node			= node;
				}
			}
		}
		bool render_options_editor::load_panels_layout( )
		{
			m_multidocument->main_dock_panel->DockRightPortion = 0.40f;
			bool is_loaded_normally = resource_editor::load_panels_layout();

			//if(is_loaded_normally)
			//	m_render_options_view_panel->Show				(m_multidocument->main_dock_panel);
			//else
			//{
				m_multidocument->main_dock_panel->DockLeftPortion = 0.25f;
				m_render_options_view_panel->Show				(m_multidocument->main_dock_panel, DockState::DockLeft);
			//}

			m_multidocument->is_single_document						= true;
			render_options_panel->tree_view->LabelEdit				= true;
			render_options_panel->tree_view->ContextMenu			= nullptr;
			render_options_panel->tree_view->ImageList				= m_images;
			render_options_panel->tree_view->is_selectable_groups	= true;
			render_options_panel->tree_view->is_multiselect			= true;

			render_options_panel->tree_view->source					= m_render_options_source;
			render_options_panel->tree_view->refresh				();

			return is_loaded_normally;
		}
		void render_options_editor::in_constructor( )
		{
			m_multidocument->Name												= "render_options_editor";
			m_render_options_view_panel											= gcnew controls::file_view_panel_base(m_multidocument);
			m_properties_panel->Text											= "Render Options";

			m_render_options_view_panel->tree_view->keyboard_search_enabled		= true;
			m_render_options_view_panel->tree_view->selected_items_changed		+= gcnew System::EventHandler<controls::tree_view_selection_event_args^>(this, &render_options_editor::on_resource_selected);
			FormClosed															+= gcnew FormClosedEventHandler(this, &render_options_editor::form_closed);
			
			m_clone_material_menu_item			= gcnew ToolStripMenuItem();
			m_clone_material_menu_item->Text	= "Clone";
			m_clone_material_menu_item->Click	+= gcnew System::EventHandler(this, &render_options_editor::clone_tool_stip_item_click);
			
			resources_tree_view->ContextMenuStrip->Items->Add( m_clone_material_menu_item );
		}
		System::String^ render_options_editor::validate_resource_name( System::String^ name_to_check )
		{
			System::String^ result = name_to_check;

			if( result == "" )
				result = "_";

			int	index		= 0;
			if( nullptr!=find_hierrarchy( m_render_options_view_panel->tree_view->nodes, result) )
			{
				do{
					++index;
				}while( nullptr!=find_hierrarchy( m_render_options_view_panel->tree_view->nodes, result+"_"+index) );

				result				= name_to_check+"_"+index;		
			}

			return result;
		}
		resource_editor_resource^ render_options_editor::create_resource( System::String^ in_name )
		{
			resource_editor_resource^ result = nullptr;
			result		= gcnew render_options_editor_resource( 
							m_editor_world,
							in_name, 
							gcnew resource_changed_delegate(this, &render_options_editor::on_resource_changed),
							gcnew resource_load_delegate(this, &render_options_editor::on_resource_loaded) );
			
			return result;
		}
		void render_options_editor::clone_tool_stip_item_click( Object^ , System::EventArgs^ )
		{
			if( m_render_options_view_panel->tree_view->selected_nodes->Count != 1 )
				return;
			
			render_options_editor_resource^ other_resource			= safe_cast<render_options_editor_resource^>( m_opened_resources[m_render_options_view_panel->tree_view->selected_nodes[0]->FullPath] );
			
			System::String^ new_resource_name	= validate_resource_name( other_resource->m_name + "_clone" );
			
			resource_editor_resource^ res	= create_resource( new_resource_name );
			res->m_name						= new_resource_name;
			res->m_old_name					= new_resource_name;
			m_opened_resources->Add			( res->m_name, res );
			res->copy_from					( other_resource );
			res->apply_changes				( );
			res->m_is_new					= true;
			
			tree_node^ node =				 m_render_options_view_panel->tree_view->add_item( new_resource_name, xray::editor_base::node_material );
			m_render_options_view_panel->tree_view->Sort			( );
			m_render_options_view_panel->tree_view->select_node( node, true, true );
			on_resource_selected			( nullptr, nullptr );
			make_wrapper_modified			( m_opened_resources[new_resource_name] );
			node->SelectedImageIndex		= editor_base::node_material_added;
			node->ImageIndex				= editor_base::node_material_added;
			node->BeginEdit( );
		}
		document_base^ render_options_editor::create_new_document( )
		{
			NOT_IMPLEMENTED();
			return nullptr;
		}
		String^ render_options_editor::selected_name::get( )
		{
			R_ASSERT("trying to receive resource name when requesting resource");
			return String::Empty;
		}
		void render_options_editor::selected_name::set( System::String^ )
		{

		}
	} // namespace editor
} // namespace xray
