////////////////////////////////////////////////////////////////////////////
//	Created		: 23.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_world.h"
#include "material_editor.h"
#include "material.h"
#include "material_instance.h"
#include "material_editor_source.h"
#include "resource_editor_resource.h"
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

ref class material_editor_contexts_manager :public xray::editor_base::i_context_manager
{
public:
					material_editor_contexts_manager	( material_editor^ ed ):m_material_editor(ed){};
	virtual bool	context_fit							( System::String^ context_str )
					{
						if(context_str=="Global")
						{
							return true;
						}else
						if(context_str=="View")
						{
							return m_material_editor->m_view_window->context_fit();
						}else
							return false;
					}

	virtual string_list^	get_contexts_list( )
					{ 
						return gcnew string_list( gcnew cli::array<System::String^>(4){"Global", "View", "FilesView", "Properties"} );
					}
	
private:
	material_editor^		m_material_editor;
};

material_editor::~material_editor( )
{
	if(m_load_preview_model_query)
		m_load_preview_model_query->m_rejected = true;

	if(m_load_preview_material_query)
		m_load_preview_material_query->m_rejected = true;
}

void material_editor::in_constructor( )
{
	m_preview_model				= NEW(render::static_model_ptr)();
	m_model_cube				= NEW(render::static_model_ptr)();
	m_model_sphere				= NEW(render::static_model_ptr)();

	m_scene						= NEW(render::scene_ptr);
	m_scene_view				= NEW(render::scene_view_ptr);
	m_output_window				= NEW(render::render_output_window_ptr);
	
	m_need_reset_grid_on_revert	= false;
	
	if( m_mode==material_editor_mode::edit_material)
	{
		Name 					= "material_editor";
		Text 					= "Material Editor";
		m_materials_source		= gcnew material_editor_source( "resources/materials", ".material" );
		m_multidocument->Name	= "material_editor";
	}else
	{
		Name 					= "material_instance_editor";
		Text 					= "Material Instance Editor";
		m_materials_source		= gcnew material_editor_source( "resources/material_instances", ".material" );
		m_multidocument->Name	= "material_instance_editor";
	}

	m_material_view_panel				= gcnew controls::file_view_panel_base(m_multidocument);
	m_properties_panel->Text			= "Properties";

	m_view_window						= gcnew editor_base::scene_view_panel;
	math::float3 eye_pos				= float3(0.f, 5.f, -5.f);
	m_view_window->setup_view_matrix	( eye_pos, float3(0.0f,  0.0f, 0.0f) );
	
	m_material_view_panel->tree_view->selected_items_changed			+= gcnew System::EventHandler<controls::tree_view_selection_event_args^>(this, &material_editor::on_resource_selected);

	m_add_material_menu_item			= gcnew ToolStripMenuItem();
	m_add_material_menu_item->Text		= "Add Material";
	m_add_material_menu_item->Click		+= gcnew System::EventHandler(this, &material_editor::add_tool_stip_item_click);

	m_remove_material_menu_item			= gcnew ToolStripMenuItem();
	m_remove_material_menu_item->Text	= "Remove";
	m_remove_material_menu_item->Click	+= gcnew System::EventHandler(this, &material_editor::remove_tool_stip_item_click);

	m_rename_material_menu_item			= gcnew ToolStripMenuItem();
	m_rename_material_menu_item->Text	= "Rename";
	m_rename_material_menu_item->Click	+= gcnew System::EventHandler(this, &material_editor::rename_tool_stip_item_click);

	m_copy_material_menu_item			= gcnew ToolStripMenuItem();
	m_copy_material_menu_item->Text		= "Copy";
	m_copy_material_menu_item->Click	+= gcnew System::EventHandler(this, &material_editor::copy_tool_stip_item_click);

	m_paste_material_menu_item			= gcnew ToolStripMenuItem();
	m_paste_material_menu_item->Text	= "Paste";
	m_paste_material_menu_item->Click	+= gcnew System::EventHandler(this, &material_editor::paste_tool_stip_item_click);
	
	m_clone_material_menu_item			= gcnew ToolStripMenuItem();
	m_clone_material_menu_item->Text	= "Clone";
	m_clone_material_menu_item->Click	+= gcnew System::EventHandler(this, &material_editor::clone_tool_stip_item_click);

	m_create_folder_menu_item			= gcnew ToolStripMenuItem();
	m_create_folder_menu_item->Text		= "Create Folder";
	m_create_folder_menu_item->Click	+= gcnew System::EventHandler(this, &material_editor::create_folder_tool_stip_item_click);

	ToolStripMenuItem^ load_all_menu_item	= gcnew ToolStripMenuItem();
	load_all_menu_item->Text			= "Load all resources";
	load_all_menu_item->Click			+= gcnew System::EventHandler(this, &material_editor::load_all_resources);

	resources_tree_view->ContextMenuStrip->Items->Add( m_add_material_menu_item );
	resources_tree_view->ContextMenuStrip->Items->Add( m_remove_material_menu_item );
	resources_tree_view->ContextMenuStrip->Items->Add( m_rename_material_menu_item );

	resources_tree_view->ContextMenuStrip->Items->Add( m_copy_material_menu_item );
	resources_tree_view->ContextMenuStrip->Items->Add( m_paste_material_menu_item );

	resources_tree_view->ContextMenuStrip->Items->Add( m_clone_material_menu_item );
	resources_tree_view->ContextMenuStrip->Items->Add( m_create_folder_menu_item );

	resources_tree_view->ContextMenuStrip->Items->Add( load_all_menu_item );
	
	m_material_view_panel->tree_view->AfterLabelEdit	+= gcnew NodeLabelEditEventHandler(this,& material_editor::item_label_renamed);
	
	m_material_view_panel->tree_view->keyboard_search_enabled = true;

	m_input_engine		= gcnew editor_base::input_engine	( editor_base::input_keys_holder::ref, gcnew material_editor_contexts_manager(this));
	m_gui_binder		= gcnew editor_base::gui_binder		( m_input_engine ); 
	m_gui_binder->set_image_list	( "base", m_images );

	// Maya style mouse actions 
	m_input_engine->register_action	( gcnew	editor_base::mouse_action_view_move_xy	( "view maya move xy",	m_view_window ), "Alt+MButton(View)" );
	m_input_engine->register_action	( gcnew	editor_base::mouse_action_view_move_z		( "view maya move z",	m_view_window ), "Alt+RButton(View)" );
	m_input_engine->register_action	( gcnew	editor_base::mouse_action_view_rotate_around( "view maya rotate",	m_view_window ), "Alt+LButton(View)" );
	
	// Old LE style mouse actions 
	m_input_engine->register_action	( gcnew	editor_base::mouse_action_view_rotate		( "view LE rotate",		m_view_window ), "Space+LButton+RButton(View)" );
	m_input_engine->register_action	( gcnew	editor_base::mouse_action_view_move_y_reverse( "view LE move y",	m_view_window ), "Space+RButton(View)" );
	m_input_engine->register_action	( gcnew	editor_base::mouse_action_view_move_xz_reverse( "view LE move xz",m_view_window ), "Space+LButton(View)" );

	m_view_window->create_tool_strip( "MainToolStrip", 0);

	System::String^ action_name		= "Focus to origin";
	m_input_engine->register_action	( gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(m_view_window, &editor_base::scene_view_panel::action_focus_to_origin) ), "F(View)" );
	m_gui_binder->register_image	( "base", action_name,		editor_base::focus_to_origin );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 1 );

	action_name						= "Refresh material";
	m_input_engine->register_action	( gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &material_editor::action_refresh_preview_material) ), "R(Global)" );
	m_gui_binder->register_image	( "base", action_name,		editor_base::reload_shaders );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 2 );

	action_name						= "Auto refresh";
	editor_base::action_delegate^ a = gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &material_editor::action_auto_refresh) );
	a->set_checked					( gcnew editor_base::checked_delegate_managed(this, &material_editor::is_auto_refresh));
	m_input_engine->register_action	( a, "" );
	m_gui_binder->register_image	( "base", action_name,		editor_base::reload_modified_textures );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 3 );
	
	action_name						= "Global auto refresh";
	a								= gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &material_editor::global_action_auto_refresh) );
	a->set_checked					( gcnew editor_base::checked_delegate_managed(this, &material_editor::is_global_auto_refresh));
	m_input_engine->register_action	( a, "" );
	m_gui_binder->register_image	( "base", action_name,		editor_base::pin );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 3 );
	
	action_name						= "Preview model cube";
	a								= gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &material_editor::action_preview_model_cube) );
	a->set_checked					( gcnew editor_base::checked_delegate_managed(this, &material_editor::is_preview_model_cube));
	m_input_engine->register_action	( a, "" );
	m_gui_binder->register_image	( "base", action_name,		editor_base::preview_cube );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 4 );
	
	action_name						= "Preview model sphere";
	a								= gcnew	editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &material_editor::action_preview_model_sphere) );
	a->set_checked					( gcnew editor_base::checked_delegate_managed(this, &material_editor::is_preview_model_sphere));
	m_input_engine->register_action	( a, "" );
	m_gui_binder->register_image	( "base", action_name,	editor_base::preview_sphere );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 5 );
	
	action_name						= "Setup shared texture";
	a								= gcnew editor_base::action_delegate( action_name, gcnew editor_base::execute_delegate_managed(this, &material_editor::action_setup_current_diffuse_texture) );
	a->set_enabled					( gcnew editor_base::enabled_delegate_managed(this, &material_editor::is_setup_current_diffuse_texture));
	m_input_engine->register_action	( a, "" );
	m_gui_binder->register_image	( "base", action_name,	editor_base::setup_shared_data );
	m_view_window->add_action_button_item( m_gui_binder, action_name, "MainToolStrip", 6 );
	
	query_create_render_resources	( );

	m_view_window->action_focus_to_bbox( math::create_identity_aabb() );
}

void material_editor::query_create_render_resources()
{
	render::editor_renderer_configuration render_configuration;
 	System::Int32 hwnd					= m_view_window->view_handle();
 	
 	resources::user_data_variant* temp_data[] = { NEW(resources::user_data_variant), 0, NEW(resources::user_data_variant)};
 	temp_data[0]->set(render_configuration);
 	temp_data[2]->set(*(HWND*)&hwnd);
 	
 	resources::user_data_variant const* data[] = {temp_data[0], temp_data[1], temp_data[2]};
 	
 	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &material_editor::on_render_resources_ready), g_allocator);
 	
 	const_buffer					temp_buffer( "", 1 );
 	resources::creation_request		requests[] = 
 	{
 		resources::creation_request( "material_editor_scene", temp_buffer, resources::scene_class ),
 		resources::creation_request( "material_editor_scene_view", temp_buffer, resources::scene_view_class ),
 		resources::creation_request( "material_editor_render_output_window", temp_buffer, resources::render_output_window_class )
 	};
  	resources::query_create_resources(
  		requests,
  		boost::bind(&query_result_delegate::callback, q, _1),
  		g_allocator,
  		data
  	);
 	DELETE(temp_data[0]);
 	DELETE(temp_data[2]);
}

void material_editor::on_render_resources_ready(resources::queries_result& data)
{
 	R_ASSERT					( data.is_successful() );
 	
 	*m_scene					= static_cast_resource_ptr<xray::render::scene_ptr>(data[0].get_unmanaged_resource());
 	*m_scene_view				= static_cast_resource_ptr<xray::render::scene_view_ptr>(data[1].get_unmanaged_resource());
 	*m_output_window			= static_cast_resource_ptr<xray::render::render_output_window_ptr>(data[2].get_unmanaged_resource());
 	
 	m_editor_renderer			= &m_editor_world.engine().get_renderer_world().editor_renderer( );
 	m_view_window->setup_scene	( m_scene->c_ptr(), m_scene_view->c_ptr(), *m_editor_renderer, true );
	query_preview_models		( );
	m_view_window->register_actions	( m_input_engine, m_gui_binder, true );
}

void material_editor::on_resource_changed()
{
	if(is_auto_refresh())
		action_refresh_preview_material();
}

void material_editor::tick( )
{
	super::tick( );
	
	if(m_need_refresh_properties)
	{
		view_selected_options_impl		( );
		m_need_refresh_properties		= false;

		if(is_auto_refresh())
			action_refresh_preview_material();
	}

	
	m_input_engine->execute( );
	m_gui_binder->update_items			( );

	xray::render::editor::renderer* r = m_view_window->editor_renderer();
	if( r )
	{
 		m_view_window->tick			( );
		r->scene().set_view_matrix			( *m_scene_view, math::invert4x3(m_view_window->get_inverted_view_matrix()) );
		r->scene().set_projection_matrix	( *m_scene_view, m_view_window->get_projection_matrix() );
		m_view_window->render		( );
		r->draw_scene				( *m_scene, *m_scene_view, *m_output_window, render::viewport_type( float2(0.f, 0.f), float2(1.f, 1.f) ) );
	}
}

void material_editor::clear_resources( )
{
	super::clear_resources	( );

	xray::render::editor::renderer* r = m_view_window->editor_renderer();
	if( r )
	{
		if( (*m_preview_model).c_ptr() )
			r->scene().remove_model ( *m_scene, (*m_preview_model)->m_render_model  );


		m_view_window->clear_resources		( );
		//m_view_window->destroy_output_window( );
		DELETE				( m_output_window );
		DELETE				( m_scene_view );
		DELETE				( m_scene );
		m_view_window->clear_scene	( );
	}
	
	DELETE					( m_preview_model );
	DELETE					( m_model_cube );
	DELETE					( m_model_sphere );
}

String^ material_editor::selected_name::get( )
{
	if(selecting_entity == entity::resource_name)
		return m_material_view_panel->tree_view->selected_nodes[0]->FullPath;
	
	R_ASSERT("trying to receive resource name when requesting resource");
	return String::Empty;
}

void material_editor::selected_name::set( System::String^ value )
{
	if(m_is_resource_list_loaded)
		m_material_view_panel->tree_view->track_active_node(value);
	else
		m_track_resource_name = value;
}

resource_editor_resource^ material_editor::select_resource_by_name( System::String^ fullpath )
{
	resource_editor_resource^ m = nullptr;
	if(!m_opened_resources->ContainsKey(fullpath))
	{
		m							= create_resource( );
		m_opened_resources->Add		( fullpath, m );
		m->load						( fullpath );//
	}else
	{
		m							= m_opened_resources[fullpath];
	}
	return m;
}

void material_editor::on_resource_selected( Object^, controls::tree_view_selection_event_args^ )
{
	m_need_refresh_properties			= false;
	m_selected_resources->Clear			( );
	controls::tree_view^ tree			= m_material_view_panel->tree_view;

	bool all_loaded						= true;

	if(tree->selected_nodes->Count > 0)
	{
		ReadOnlyCollection<tree_node^>^ selected_tree_items = tree->selected_nodes;
		
		for each(controls::tree_node^ node in selected_tree_items)
		{
			System::String^ fullpath = node->FullPath;

			if(node->m_node_type == controls::tree_node_type::single_item)
			{
				resource_editor_resource^ m = select_resource_by_name( fullpath );
				m_selected_resources->Add	( m );
				all_loaded					&= m->loaded;
			}
		}
	}

	if(all_loaded)
		m_need_refresh_properties = true;
}

void material_editor::on_resource_changed( resource_editor_resource^ data )
{
	make_wrapper_modified	( data );
	on_resource_loaded		( data, true );
}

void material_editor::on_resource_loaded( resource_editor_resource^ resource, bool is_correct )
{
	R_ASSERT(m_opened_resources->ContainsValue(resource));
	
	if(!m_selected_resources->Contains(resource))
		return;

	bool all_objects_loaded = true;

	for each ( resource_editor_resource^ r in m_selected_resources )
		all_objects_loaded	&= r->loaded;

	if(!is_correct)
		raise_property_value_changed();
	
	if(all_objects_loaded)
		m_need_refresh_properties = true;
}

void material_editor::load_all_resources( Object^, System::EventArgs^ )
{
	for each(System::String^ resource_name in m_materials_source->m_items)
		select_resource_by_name( resource_name );
}

void material_editor::add_tool_stip_item_click( Object^, System::EventArgs^ )
{
	System::String^ selected_path = "";
	if( m_material_view_panel->tree_view->selected_nodes->Count !=0 )
	{
		tree_node^ n = m_material_view_panel->tree_view->selected_nodes[0];
		if(n->m_node_type == controls::tree_node_type::group_item)
			selected_path = n->FullPath + "/";
	}

	System::String^ new_material_name		= selected_path + "new_material";

	if( nullptr!=find_hierrarchy( m_material_view_panel->tree_view->nodes, new_material_name)  )
	{
		int	index					= 0;
		do{
			++index;
		}while(nullptr!=find_hierrarchy( m_material_view_panel->tree_view->nodes, new_material_name+index));

		new_material_name				= new_material_name+index;		
	}
	
	resource_editor_resource^ r		= create_resource( );
	r->m_name						= new_material_name;
	r->m_old_name					= new_material_name;
	m_opened_resources->Add			( r->m_name, r );
	r->init_new						( );
	r->apply_changes				( );
	r->m_is_new						= true;

	tree_node^ node =				 m_material_view_panel->tree_view->add_item( new_material_name, editor_base::node_material );
	m_material_view_panel->tree_view->Sort			( );
	m_material_view_panel->tree_view->SelectedNode	= node;

	make_wrapper_modified			( m_opened_resources[new_material_name] );
	node->SelectedImageIndex		= editor_base::node_material_added;
	node->ImageIndex				= editor_base::node_material_added;
	node->Selected					= true;
	node->EnsureVisible				( );
	node->BeginEdit					( );
}

void material_editor::item_label_renamed( Object^ , NodeLabelEditEventArgs^ e )
{
	e->CancelEdit					= true;

	if(e->Label == nullptr || e->Label == "")
		return;

	TreeNode^ node = e->Node;

	tree_node^ n = safe_cast<tree_node^>(node);

	System::String^ old_name_full	= node->FullPath;
	System::String^ new_name_full	= (node->Parent)? node->Parent->FullPath + "/" + e->Label : e->Label;
	
	new_name_full				= validate_resource_name(new_name_full)->ToLower();
	node->Name					= IO::Path::GetFileName(new_name_full);
	node->Text					= node->Name;

	if(n->m_node_type==controls::tree_node_type::group_item)
		return;

	resource_editor_resource^ r	= m_opened_resources[old_name_full];
	r->m_name					= new_name_full;
	r->m_is_renamed				= true;
	m_opened_resources->Remove	( old_name_full );

	for(int i=0; i<m_changed_resources->Count; ++i)
	{
		if(m_changed_resources[i]->m_name == old_name_full )
		{
			m_changed_resources->RemoveAt( i );
			break;
		}
	}

	m_opened_resources->Add		( new_name_full, r );
	make_wrapper_modified		( r );

	if(r->m_is_new)
	{
		e->Node->SelectedImageIndex		= xray::editor_base::node_material_added;
		e->Node->ImageIndex				= xray::editor_base::node_material_added;
	}
}

void material_editor::remove_tool_stip_item_click( Object^ ,System::EventArgs^ )
{
	int count					= m_material_view_panel->tree_view->selected_nodes->Count;
	List<tree_node^>^ nodes		= gcnew List<tree_node^>( );
	nodes->AddRange				( m_material_view_panel->tree_view->selected_nodes );
	for(int i=count-1; i>=0; --i)
	{
		controls::tree_node^ node	= safe_cast<controls::tree_node^>( nodes[i] );
		if( node->TreeView == nullptr )
			continue;

		System::String^ fullpath	= node->FullPath;
		if(node->m_node_type==controls::tree_node_type::single_item)
		{
			make_wrapper_modified		( m_opened_resources[fullpath] );
			node->SelectedImageIndex	= xray::editor_base::node_material_deleted;
			node->ImageIndex			= xray::editor_base::node_material_deleted;
			resource_editor_resource^ mtrl	= m_opened_resources[node->FullPath];
			mtrl->m_is_deleted				= true;
		}
		
	}
}

void material_editor::rename_tool_stip_item_click( Object^ ,System::EventArgs^ )
{
	if( m_material_view_panel->tree_view->selected_nodes->Count > 0 )
		m_material_view_panel->tree_view->selected_nodes[0]->BeginEdit( );
}

System::String^ material_editor::validate_resource_name( System::String^ name_to_check )
{
	System::String^ result = name_to_check;

	if( result == "" )
		result = "_";

	int	index		= 0;
	if( nullptr!=find_hierrarchy( m_material_view_panel->tree_view->nodes, result) )
	{
		do{
			++index;
		}while( nullptr!=find_hierrarchy( m_material_view_panel->tree_view->nodes, result+"_"+index) );

		result				= name_to_check+"_"+index;		
	}

	return result;
}

void material_editor::clone_tool_stip_item_click( Object^ , System::EventArgs^ )
{
	if( m_material_view_panel->tree_view->selected_nodes->Count != 1 )
		return;

	material^ other_material			= safe_cast<material^>( m_opened_resources[m_material_view_panel->tree_view->selected_nodes[0]->FullPath] );

	System::String^ new_material_name	= validate_resource_name( other_material->m_name + "_clone" );

	resource_editor_resource^ mtrl	= create_resource( );
	mtrl->m_name					= new_material_name;
	mtrl->m_old_name				= new_material_name;
	m_opened_resources->Add			( mtrl->m_name, mtrl );
	mtrl->copy_from					( other_material );
	mtrl->apply_changes				( );
	mtrl->m_is_new					= true;

	tree_node^ node =				 m_material_view_panel->tree_view->add_item( new_material_name, xray::editor_base::node_material );
	m_material_view_panel->tree_view->Sort			( );
	m_material_view_panel->tree_view->select_node( node, true, true );
	on_resource_selected			( nullptr, nullptr );
	make_wrapper_modified			( m_opened_resources[new_material_name] );
	node->SelectedImageIndex		= editor_base::node_material_added;
	node->ImageIndex				= editor_base::node_material_added;
	node->BeginEdit( );
}

void material_editor::copy_tool_stip_item_click( Object^ , System::EventArgs^ )
{
	if( m_material_view_panel->tree_view->selected_nodes->Count != 1 )
		return;

	tree_node^ n					= m_material_view_panel->tree_view->selected_nodes[0];
	material^ m						= safe_cast<material^>( m_opened_resources[n->FullPath] );

	configs::lua_config_ptr config	= configs::create_lua_config( );
	configs::lua_config_value root	= config->get_root();

	strings::stream stream			( g_allocator );
	stream.append					( "%s", (m_mode==material_editor_mode::edit_material) ? "material" : "instance" );
	(*m->material_config())->get_root()["clipboard_name"] = unmanaged_string( n->Text ).c_str();
	(*m->material_config())->save	( stream );
	(*m->material_config())->get_root().erase("clipboard_name");
	stream.append					( "%c", 0 );

	os::copy_to_clipboard			( (pcstr)stream.get_buffer() );
}

void material_editor::paste_tool_stip_item_click( Object^ , System::EventArgs^ )
{
	pstr string				= os::paste_from_clipboard( *g_allocator );
	// clipboard doesn't contain text information
	if ( !string ) 
	{
		// we could use function os::is_clipboard_empty() to check
		// whether there non-text information and show some message in that case
		return;
	}

	material_editor_mode mode = material_editor_mode::unknown;
	int offset = 0;
	if( strings::starts_with(string, "material") )
	{
		mode = material_editor_mode::edit_material;
		offset = strings::length("material");
	}else
	if( strings::starts_with(string, "instance") )
	{
		mode = material_editor_mode::edit_instances;
		offset = strings::length("instance");
	}else
	{
		FREE		( string );
		return;
	}

	configs::lua_config_ptr config	= configs::create_lua_config_from_string( string+offset );
	FREE							( string );
	if(mode!=m_mode)
		return;

	// clipboard does contain text information
	if ( !config ) {
		// but there are no valid lua script in clipboard
		// we could show message about this
		return;
	}

	System::String^ node_path		= "";
	tree_node^ n = m_material_view_panel->tree_view->selected_node;
	if(n)
	{
		while(n && n->m_node_type!=controls::tree_node_type::group_item )
			n = n->Parent;

		if(n)
		{
			node_path = n->FullPath;
			node_path	+= "/";
		}
	}
	
	pcstr stored_name				= config->get_root()["clipboard_name"];
	node_path						+= "pasted_";
	node_path						+= gcnew System::String( stored_name );
	config->get_root().erase("clipboard_name");

	System::String^ new_material_name	= validate_resource_name( node_path );
	material^ mtrl					= safe_cast<material^>(create_resource( ));
	mtrl->m_name					= new_material_name;
	mtrl->m_old_name				= new_material_name;
	m_opened_resources->Add			( mtrl->m_name, mtrl );
	mtrl->create_from				( config->get_root() );
	mtrl->apply_changes				( );
	mtrl->m_is_new					= true;

	tree_node^ node =				 m_material_view_panel->tree_view->add_item( new_material_name, xray::editor_base::node_material );
	m_material_view_panel->tree_view->Sort			( );
	m_material_view_panel->tree_view->select_node( node, true, true );
	on_resource_selected			( nullptr, nullptr );
	make_wrapper_modified			( m_opened_resources[new_material_name] );
	node->SelectedImageIndex		= editor_base::node_material_added;
	node->ImageIndex				= editor_base::node_material_added;
	node->EnsureVisible				( );
	node->BeginEdit					( );
}

void material_editor::create_folder_tool_stip_item_click( Object^ , System::EventArgs^ )
{
	System::String^ path		= "";
	tree_node^ n = m_material_view_panel->tree_view->selected_node;
	if(n)
	{
		while(n && n->m_node_type!=controls::tree_node_type::group_item )
			n = n->Parent;

		if(n)
		{
			path = n->FullPath;
			path	+= "/";
		}
	}

	path						= validate_resource_name(path+"new_folder")->ToLower();

	tree_node^ new_node = m_material_view_panel->tree_view->add_group(path, editor_base::folder_closed, editor_base::folder_open );

	new_node->EnsureVisible				( );
	new_node->BeginEdit					( );
}

bool material_editor::load_panels_layout( )
{
	m_multidocument->main_dock_panel->DockRightPortion = 0.45f;
	bool is_loaded_normally = resource_editor::load_panels_layout();

	if(is_loaded_normally)
	{
		m_material_view_panel->Show		( m_multidocument->main_dock_panel );
		m_view_window->Show				( m_multidocument->main_dock_panel );
	}else
	{
		m_multidocument->main_dock_panel->DockLeftPortion = 0.25f;
		m_material_view_panel->Show		( m_multidocument->main_dock_panel, DockState::DockLeft );
		m_view_window->Show				( m_multidocument->main_dock_panel, DockState::Document );
	}

	m_multidocument->is_single_document						= true;
	m_material_view_panel->tree_view->LabelEdit				= true;
	m_material_view_panel->tree_view->ContextMenu			= nullptr;
	m_material_view_panel->tree_view->ImageList				= m_images;
	m_material_view_panel->tree_view->is_selectable_groups	= true;
	m_material_view_panel->tree_view->is_multiselect		= true;

	m_material_view_panel->tree_view->source				= m_materials_source;
	m_material_view_panel->tree_view->refresh				( );

	return is_loaded_normally;
}

IDockContent^ material_editor::layout_for_panel( System::String^ panel_name )
{
	if(panel_name == "xray.editor.controls.file_view_panel_base")
		return m_material_view_panel;

	return resource_editor::layout_for_panel(panel_name);
}

document_base^ material_editor::create_new_document( )
{
	NOT_IMPLEMENTED();
	return nullptr;
}

void material_editor::on_menu_opening( System::Object^ sender, System::ComponentModel::CancelEventArgs^ e )
{
	super::on_menu_opening( sender, e );
	
	bool selection_not_empty			= ( m_selected_resources->Count != 0 );
	revert_toolStripMenuItem->Visible	= selection_not_empty;
	applyToolStripMenuItem->Visible		= selection_not_empty;

	if( selection_not_empty )
	{
		bool can_revert_or_apply	= false;
		for each ( resource_editor_resource^ r in m_selected_resources )
		{
			if( m_changed_resources->Contains( r ) )
			{
				can_revert_or_apply = true;
				break;
			}
		}
		revert_toolStripMenuItem->Enabled		= can_revert_or_apply;
		applyToolStripMenuItem->Enabled			= can_revert_or_apply;
	}
	
	m_clone_material_menu_item->Visible		= ( m_selected_resources->Count == 1 );
	m_rename_material_menu_item->Visible	= ( m_selected_resources->Count == 1 );
	m_copy_material_menu_item->Visible		= ( m_selected_resources->Count == 1 );

	m_paste_material_menu_item->Visible		= !os::is_clipboard_empty( ) && !selection_not_empty;
	m_remove_material_menu_item->Visible	= selection_not_empty;
		
	m_add_material_menu_item->Visible		= !selection_not_empty;
	m_create_folder_menu_item->Visible		= !selection_not_empty;
}

void material_editor::load_settings( RegistryKey^ product_key ) 
{ 
	RegistryKey^ editor_key		= xray::base_registry_key::get_sub_key(product_key, Name );

	super::load_settings		( editor_key );
	m_view_window->load_settings( editor_key );
	m_auto_refresh				= System::Convert::ToBoolean( editor_key->GetValue("auto_refresh", false) );
	m_global_auto_refresh		= System::Convert::ToBoolean( editor_key->GetValue("global_auto_refresh", false) );
	m_is_apply_resource_on_property_value_changed = m_global_auto_refresh;
	
	editor_key->Close			( );
}

void material_editor::save_settings( RegistryKey^ product_key ) 
{ 
	RegistryKey^ editor_key		= xray::base_registry_key::get_sub_key(product_key, Name );
	
	super::save_settings		( editor_key );
	m_view_window->save_settings( editor_key );
	editor_key->SetValue		( "auto_refresh", m_auto_refresh );
	editor_key->SetValue		( "global_auto_refresh", m_global_auto_refresh );
	
	editor_key->Close			( );
}


} // namespace editor
} // namespace xray
