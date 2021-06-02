/-------------------------------------------------------------------------------------------
//	Created		: 23.12.2009
//	Author		: Alexander Plichko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_editor.h"
#include "particle_graph_document.h"
#include "particle_groups_source.h"
#include "particle_graph_node_style.h"
#include "particle_graph_node.h"
#include "particle_graph_node_action.h"
#include "particle_graph_node_emitter.h"
#include "particle_graph_node_event.h"
#include "particle_graph_node_property.h"
#include "particle_node_properties.h"
#include "particle_source_nodes_container.h"
#include "particle_sources_panel.h"
#include "particle_tree_node_holder.h"
#include "images/particle_editor_images16x16.h"
#include "particle_system_config_wrapper.h"
#include "particle_time_based_layout_panel.h"
#include "particle_data_sources_panel.h"
#include "particle_contexts_manager.h"
#include "disable_group_nodes.h"
#include "transform_control_helper.h"
#include "input_actions.h"

#pragma managed( push, off )
#include <xray/strings_stream.h>
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/scene_view_mode.h>
#include <xray/render/facade/render_stage_types.h>
#include <xray/render/facade/editor_renderer.h>
#include <xray/collision/api.h>
#include <xray/ui/world.h>
#include <xray/particle/world.h>
#include <xray/resources.h>
#include <xray/resources_query_result.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/render/engine/base_classes.h>
#pragma managed( pop )

using namespace System::Windows;

namespace xray {
namespace particle_editor {
	static bool process_clipboard( )
	{
		//context_menu->Items[1]->Enabled = false;
		pstr string				= xray::os::paste_from_clipboard( *g_allocator );
		// clipboard doesn't contain text information


		if ( !string )
			return false;

		bool is_copy	= xray::strings::starts_with( string, "tree_view_clipboard_copy");
		bool is_cut		= xray::strings::starts_with( string, "tree_view_clipboard_cut");
		
		FREE					( string );

		if (is_copy || is_cut)	{
			return true;
		}
		else{
			return false;
		}		
	}

	static void check_for_opened_docs_source_existance(particle_editor^ editor)
	{
		List<document_base^>^	docs_to_close = gcnew List<document_base^>( );
		for each( document_base^ doc in editor->multidocument_base->opened_documents )
		{
			String^ file_path	= particle_editor::absolute_particle_resources_path+"particles/"+doc->source_path+safe_cast<particle_graph_document^>(doc)->document_extension;
		
			if (!IO::File::Exists(file_path))
				docs_to_close->Add(doc);				
		}
		for each(document_base^ doc in docs_to_close)
		{
			doc->Close();
		}
	}

	static String^ replace_first(String^ source, String^ old_value, String^ new_value)
	{
		int old_start_index = source->IndexOf(old_value);
		source = source->Remove(old_start_index, old_value->Length);
		source = source->Insert(old_start_index, new_value);
		return source;
	}
	static void check_for_opened_docs_names_on_rename(
								particle_editor^	editor,
								String^				old_document_name,
								String^				new_document_name
								)
	{
		String^ particles_string = gcnew String("particles/");
		old_document_name = old_document_name->Substring(particles_string->Length, old_document_name->Length - particles_string->Length);
		new_document_name = new_document_name->Substring(particles_string->Length, new_document_name->Length - particles_string->Length);
		
		


		for each(document_base^ doc in editor->multidocument_base->opened_documents)
		{
			if (doc->source_path->Contains(old_document_name))
			{
				doc->source_path = replace_first(doc->source_path, old_document_name, new_document_name);
				doc->Text = replace_first(doc->Text, old_document_name, new_document_name);
				doc->Name = replace_first(doc->Name, old_document_name, new_document_name);
			}
		
		}
	}

particle_editor::particle_editor(	System::String^ res_path, 
									render::world& render_world, 
									xray::ui::world& ui_world,
									xray::editor_base::tool_window_holder^ h )
:m_render_world		( render_world ),
m_ui_world			( ui_world ),
m_editor_renderer	( 0 ),
m_holder			( h ),
m_closed			( false ),
m_current_play_speed( 1.0f ),
m_is_playing		( true ),
m_is_looping_particle_system( true ),
m_is_use_post_process( true ),
m_is_use_skybox		( true )
{
	particle_resources_path				= "resources";
	absolute_particle_resources_path	= res_path+"/sources/";

	m_scene								= NEW( render::scene_ptr );
	m_scene_view						= NEW( render::scene_view_ptr );
	m_output_window						= NEW(xray::render::render_output_window_ptr);
	m_holder->register_tool_window		( this );
	initialize							( );
}

particle_editor::~particle_editor( )	
{
	m_holder->unregister_tool_window( this );
	DELETE	( m_scene_view );
	DELETE	( m_scene );
}

void particle_editor::query_create_render_resources()
{
	render::editor_renderer_configuration render_configuration;
	render_configuration.m_create_particle_world		= true;
	
	System::Int32 hwnd					= m_view_window->view_handle();
	
	resources::user_data_variant* temp_data[] = { NEW(resources::user_data_variant), 0, NEW(resources::user_data_variant)};
	temp_data[0]->set(render_configuration);
	temp_data[2]->set(*(HWND*)&hwnd);
	
	resources::user_data_variant const* data[] = {temp_data[0], temp_data[1], temp_data[2]};
	
	query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &particle_editor::on_render_resources_ready), g_allocator);
	
	const_buffer					temp_buffer( "", 1 );
	resources::creation_request		requests[] = 
	{
		resources::creation_request( "particle_editor_scene", temp_buffer, resources::scene_class ),
		resources::creation_request( "particle_editor_scene_view", temp_buffer, resources::scene_view_class ),
		resources::creation_request( "particle_editor_render_output_window", temp_buffer, resources::render_output_window_class )
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

void particle_editor::on_render_resources_ready(resources::queries_result& data)
{
	ASSERT(data.is_successful());
	
	*m_scene					= static_cast_resource_ptr<xray::render::scene_ptr>(data[0].get_unmanaged_resource());
	*m_scene_view				= static_cast_resource_ptr<xray::render::scene_view_ptr>(data[1].get_unmanaged_resource());
	*m_output_window			= static_cast_resource_ptr<xray::render::render_output_window_ptr>(data[2].get_unmanaged_resource());
	m_editor_renderer			= &m_render_world.editor_renderer( );
	
	m_view_window->setup_scene	( m_scene->c_ptr(), m_scene_view->c_ptr(), *m_editor_renderer, true  );

	m_transform_control_helper						= gcnew pe_transform_control_helper(this);
	m_transform_control_helper->m_collision_tree	= m_view_window->m_collision_tree;
	m_transform_control_helper->m_editor_renderer	= m_editor_renderer;
	register_actions								( );
	m_view_window->register_actions					( m_input_engine, m_gui_binder , true);

	m_particles_panel->refresh								();
	load_particles_library();

}

static void draw_text(xray::ui::world& ui_world, pcstr str, float pos_x, float pos_y, u32 clr)
{
	float2 line_size		(100.0f, 10.0f);
	
	xray::ui::text* txt		= ui_world.create_text();
	txt->set_text			( str );
	txt->set_font			( ui::fnt_arial );
	txt->set_text_mode		( ui::tm_default );
	line_size.x				= math::max(line_size.x, txt->measure_string().x);
	txt->w()->set_size		( line_size );
	txt->w()->set_position	( float2(pos_x, pos_y) );
	txt->w()->set_visible	( true );
	
	txt->set_color			( clr );
	
	txt->set_text(str);
	
	txt->w()->tick();
	txt->w()->draw(ui_world.get_renderer(), ui_world.get_scene_view() );
	
	ui_world.destroy_window(txt->w());
}

void particle_editor::draw_statistics()
{
	particle_graph_document^ doc = safe_cast<particle_graph_document^>(m_multidocument_base->active_document);
	
	if (!doc)
		return;
	
	u32 num_fields = doc->get_num_info_fields();
	
	if (!num_fields)
		return;
	
	xray::particle::preview_particle_emitter_info* props = XRAY_ALLOC_IMPL(g_allocator, xray::particle::preview_particle_emitter_info, num_fields);
	doc->gather_statistics(props);
	
	float2 line_size		(100.0f, 10.0f);
	float pos				= 20.0f;
	
	u32 total_num_live_particles = 0;
	u32 total_num_current_max_particles = 0;
	
	for (u32 i=0; i<num_fields; i++)
	{
		total_num_live_particles += props[i].num_live_particles;
		total_num_current_max_particles += props[i].num_current_max_particles;
	}
	
	for (u32 i=0; i<num_fields; i++)
	{
		fixed_string<256> buffer;
		buffer.assignf("%d: %d/%d, %.2f/%.2f, %d",
			i,
			props[i].num_live_particles,
			props[i].num_current_max_particles,
			props[i].current_emitter_time,// * props[i].emitter_duration,
			props[i].emitter_duration,
			props[i].num_fire_events
		);
		
		draw_text(m_ui_world, buffer.c_str(), 20.0f, pos, 0xffffffff);
		
		pos += 20.0f;
	}
	
	fixed_string<256> total_particles_str;
	total_particles_str.assignf("%d/%d", total_num_live_particles, total_num_current_max_particles);
	
	draw_text(m_ui_world, total_particles_str.c_str(), 20.0f, pos, 0xffccffcc);
	
	XRAY_FREE_IMPL					( g_allocator, props );
}

void particle_editor::tick() 
{
	if (!m_editor_renderer)
		return;

	m_input_engine->execute( );

	m_view_window->tick	( );
	m_gui_binder->update_items			( );

	m_editor_renderer->scene().set_view_matrix		( *m_scene_view, math::invert4x3( m_view_window->get_inverted_view_matrix()) );
	m_editor_renderer->scene().set_projection_matrix( *m_scene_view, m_view_window->get_projection_matrix() );
	
	m_view_window->render			( );
//	m_editor_renderer->flush		( *m_scene_view, *m_output_window, render::viewport_type( float2(0.f,0.f), float2(1.f,1.f) ) );
	draw_statistics					( );
	
	if(active_control)
	{
		active_control->update		( );
		active_control->draw		( m_scene_view->c_ptr(), m_output_window->c_ptr() );
	}
	
	// update visualizers
	if(m_multidocument_base->active_document!=nullptr)
	{
		particle_graph_document^ doc = safe_cast<particle_graph_document^>(m_multidocument_base->active_document);
		doc->draw					( );
	}
	m_editor_renderer->draw_scene		( *m_scene, *m_scene_view, *m_output_window, render::viewport_type( float2(0.f,0.f), float2(1.f,1.f) ) );
}

void particle_editor::initialize( )
{
	m_available_data_source_types					= gcnew data_source_types_dictioonary( );

	m_form											= gcnew Windows::Forms::Form( );
	m_name											= "particle_editor";
	m_form->Name									= m_name;
	m_form->Text									= m_name;
	m_form->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &particle_editor::particle_editor_FormClosing);
	m_form->ShowIcon								= false;
	m_form->ShowInTaskbar							= false;

	m_node_style_mgr								= gcnew particle_graph_node_style_mgr();
	
	m_multidocument_base							= gcnew document_editor_base("particle_editor");
	m_multidocument_base->content_reloading			= gcnew content_reload_callback(this, &particle_editor::find_dock_content);
	m_multidocument_base->create_base_panel_objects = false;
	m_multidocument_base->creating_new_document		= gcnew document_create_callback(this, &particle_editor::on_create_document);

	//m_multidocument_base->newToolStripMenuItem->Click -= gcnew System::EventHandler(m_multidocument_base, &document_editor_base::new_document);
	//m_multidocument_base->newToolStripMenuItem->Click += gcnew System::EventHandler(this, &particle_editor::on_create_new_file_from_tool_strip);
	//m_multidocument_base->exitToolStripMenuItem->Click -= gcnew System::EventHandler(m_multidocument_base, &document_editor_base::exit_editor);
	//m_multidocument_base->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &particle_editor::exitToolStripMenuItem_Click);
	
	m_form->Controls->Add							( m_multidocument_base );
	m_time_layout_panel								= gcnew particle_time_based_layout_panel(this);

	m_properties_panel								= gcnew xray::editor::controls::item_properties_panel_base(multidocument_base);
	m_particles_panel								= gcnew xray::editor::controls::file_view_panel_base(multidocument_base);
	m_toolbar_panel									= gcnew xray::editor::controls::toolbar_panel_base(multidocument_base);
	m_toolbar_panel->Width							= 100;
		
	//m_particles_panel->HideOnClose	= true;
	//m_properties_panel->HideOnClose = true;
	//m_toolbar_panel->HideOnClose	= true;



	m_toolbar_panel->AutoSize		= true;
	m_toolbar_panel->AutoScale		= true;
	m_toolbar_panel->AutoScaleMode	= AutoScaleMode::Inherit;

	m_multidocument_base->Dock						= DockStyle::Fill;


	xray::editor_base::image_loader	loader;
	System::Windows::Forms::ImageList^ shared_images = loader.get_shared_images16();
 	
	//set properties
	m_particles_panel->tree_view->AfterLabelEdit			+= gcnew NodeLabelEditEventHandler(this, &particle_editor::on_tree_view_label_edit);
	
	m_particles_panel->tree_view->ShowRootLines				= true;
	m_particles_panel->tree_view->ImageList					= shared_images;
	m_particles_panel->tree_view->items_loaded				+= gcnew EventHandler(this, &particle_editor::tree_loaded);
	m_particle_groups_source								= gcnew particle_groups_source(this);
	m_particles_panel->files_source							= m_particle_groups_source;

	//m_particles_panel->tree_view->KeyDown					+= gcnew KeyEventHandler(this, &particle_editor::on_tree_view_key_down);
	
	m_particles_panel->tree_view->BeforeExpand				+= gcnew TreeViewCancelEventHandler(this, &particle_editor::on_tree_view_expand_node);

	m_toolbar_panel->SuspendLayout();



	m_toolbar_panel->Text		= L"Particle Entities";

	m_toolbar_panel->AllowDrop						= true;
	m_form->AllowDrop														= true;
	
	m_toolbar_panel->ResumeLayout(true);
	m_toolbar_panel->PerformLayout();
	
	m_data_sources_panel = gcnew particle_data_sources_panel(this);

	m_view_window			= gcnew xray::editor_base::scene_view_panel();
	math::float3 eye_pos	= float3(0.f, 5.f, -5.f);
	m_view_window->setup_view_matrix( eye_pos, float3(0.0f,  0.0f, 0.0f) );
	
	bool is_loaded_successfully = dock_panels();
	
	if(is_loaded_successfully)
	{
		if (m_properties_panel->DockHandler->DockState != WeifenLuo::WinFormsUI::Docking::DockState::Hidden)
			m_properties_panel->Show(m_multidocument_base->main_dock_panel);		
		if (m_particles_panel->DockHandler->DockState != WeifenLuo::WinFormsUI::Docking::DockState::Hidden)
			m_particles_panel->Show(m_multidocument_base->main_dock_panel);		
		if (m_toolbar_panel->DockHandler->DockState != WeifenLuo::WinFormsUI::Docking::DockState::Hidden)
			m_toolbar_panel->Show(m_multidocument_base->main_dock_panel);
		if (m_data_sources_panel->DockHandler->DockState != WeifenLuo::WinFormsUI::Docking::DockState::Hidden)
			m_data_sources_panel->Show(m_multidocument_base->main_dock_panel);
		if (m_view_window->DockHandler->DockState != WeifenLuo::WinFormsUI::Docking::DockState::Hidden)
			m_view_window->Show(m_multidocument_base->main_dock_panel);
		if (m_time_layout_panel->DockHandler->DockState != WeifenLuo::WinFormsUI::Docking::DockState::Hidden)
			m_time_layout_panel->Show(m_multidocument_base->main_dock_panel);		
	}
	else
	{			
		m_toolbar_panel->Show(m_multidocument_base->main_dock_panel, DockState::DockRight);
		m_particles_panel->Show(m_multidocument_base->main_dock_panel, DockState::DockLeft);
		m_properties_panel->Show( m_particles_panel->Pane, DockAlignment::Bottom, .5f );
		m_data_sources_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom );
		m_time_layout_panel->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom );
		m_view_window->Show(m_multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::Float );		
	}
	

	particle_configs = gcnew Generic::Dictionary<String^ , particle_system_config_wrapper^>();

	// 
	// ToolsMenuItem
	// 

	this->ToolsMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->optionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());

	this->ToolsMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->optionsToolStripMenuItem});
	this->ToolsMenuItem->Name = L"ToolsMenuItem";
	this->ToolsMenuItem->Text = L"Tools";
	this->ToolsMenuItem->Tag = L"30";

	// 
	// optionsToolStripMenuItem
	// 
	this->optionsToolStripMenuItem->Name = L"optionsToolStripMenuItem";
	this->optionsToolStripMenuItem->Text = L"Options";
	this->optionsToolStripMenuItem->Tag = L"10";
	this->optionsToolStripMenuItem->Click += gcnew System::EventHandler(this, &particle_editor::show_options);
	
	m_multidocument_base->menuStrip1->Items->Add(this->ToolsMenuItem);
	

	statusStrip = (gcnew System::Windows::Forms::StatusStrip());
	toolStripStatusLabel0 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
	toolStripStatusLabel0->Text = "Keys: [None]";
	statusStrip->Items->Add(toolStripStatusLabel0);
	m_form->Controls->Add(this->statusStrip);

	
	view_menu_item				= gcnew System::Windows::Forms::ToolStripMenuItem();
	toolbar_panel_menu_item		= gcnew System::Windows::Forms::ToolStripMenuItem();
	file_view_panel_menu_item	= gcnew System::Windows::Forms::ToolStripMenuItem();
	properties_panel_menu_item	= gcnew System::Windows::Forms::ToolStripMenuItem();
	data_sources_menu_item		= gcnew System::Windows::Forms::ToolStripMenuItem();
	preview_panel_menu_item		= gcnew System::Windows::Forms::ToolStripMenuItem();
	time_layout_menu_item		= gcnew System::Windows::Forms::ToolStripMenuItem();
	add_preview_model_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	clear_preview_model_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();

	toolbar_panel_menu_item->Checked = true;
	toolbar_panel_menu_item->CheckOnClick = true;
	toolbar_panel_menu_item->Name = L"control_panel_menu_item";
	toolbar_panel_menu_item->Size = System::Drawing::Size(197, 22);
	toolbar_panel_menu_item->Text = L"Particle Entities";
	toolbar_panel_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_toolbar_panel_menu_select);

	file_view_panel_menu_item->Checked = true;
	file_view_panel_menu_item->CheckOnClick = true;
	file_view_panel_menu_item->Name = L"file_view_panel_menu_item";
	file_view_panel_menu_item->Size = System::Drawing::Size(197, 22);
	file_view_panel_menu_item->Text = L"File viewer";
	file_view_panel_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_file_view_panel_menu_select);

	properties_panel_menu_item->Checked = true;
	properties_panel_menu_item->CheckOnClick = true;
	properties_panel_menu_item->Name = L"properties_panel_menu_item";
	properties_panel_menu_item->Size = System::Drawing::Size(197, 22);
	properties_panel_menu_item->Text = L"Item properties";
	properties_panel_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_properties_panel_menu_select);

	data_sources_menu_item->Checked = true;
	data_sources_menu_item->CheckOnClick = true;
	data_sources_menu_item->Name = L"data_sources_menu_item";
	data_sources_menu_item->Size = System::Drawing::Size(197, 22);
	data_sources_menu_item->Text = L"Data Sources";
	data_sources_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_data_sources_menu_select);

	preview_panel_menu_item->Checked = true;
	preview_panel_menu_item->CheckOnClick = true;
	preview_panel_menu_item->Name = L"preview_panel_menu_item";
	preview_panel_menu_item->Size = System::Drawing::Size(197, 22);
	preview_panel_menu_item->Text = L"View Port";
	preview_panel_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_preview_panel_menu_select);
	
	time_layout_menu_item->Checked = true;
	time_layout_menu_item->CheckOnClick = true;
	time_layout_menu_item->Name = L"time_layout_menu_item";
	time_layout_menu_item->Size = System::Drawing::Size(197, 22);
	time_layout_menu_item->Text = L"Time Layout";
	time_layout_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_time_layout_menu_select);


	view_menu_item->Name = L"view_menu_item";
	view_menu_item->Size = System::Drawing::Size(67, 20);
	view_menu_item->Text = L"&View";
	view_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_view_menu_item_select);

	view_menu_item->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
			toolbar_panel_menu_item, 
			file_view_panel_menu_item, 
			properties_panel_menu_item, 
			data_sources_menu_item,
			preview_panel_menu_item,
			time_layout_menu_item
	});

	m_multidocument_base->add_menu_items(view_menu_item);

	add_preview_model_menu_item->Name = L"add_preview_model_menu_item";
	add_preview_model_menu_item->Size = System::Drawing::Size(197, 22);
	add_preview_model_menu_item->Text = L"Add preview model...";
	add_preview_model_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_add_preview_model_menu_item_click);

	clear_preview_model_menu_item->Name = L"add_preview_model_menu_item";
	clear_preview_model_menu_item->Size = System::Drawing::Size(197, 22);
	clear_preview_model_menu_item->Text = L"Clear preview model";
	clear_preview_model_menu_item->Click += gcnew System::EventHandler(this, &particle_editor::on_clear_preview_model_menu_item_click);
	
	m_multidocument_base->fileToolStripMenuItem->DropDownItems->Insert( m_multidocument_base->fileToolStripMenuItem->DropDownItems->Count - 1, gcnew System::Windows::Forms::ToolStripSeparator());
	m_multidocument_base->fileToolStripMenuItem->DropDownItems->Insert( m_multidocument_base->fileToolStripMenuItem->DropDownItems->Count - 1, add_preview_model_menu_item );
	m_multidocument_base->fileToolStripMenuItem->DropDownItems->Insert( m_multidocument_base->fileToolStripMenuItem->DropDownItems->Count - 1, clear_preview_model_menu_item );
	m_multidocument_base->fileToolStripMenuItem->DropDownItems->Insert( m_multidocument_base->fileToolStripMenuItem->DropDownItems->Count - 1, gcnew System::Windows::Forms::ToolStripSeparator());
	
	ToolStrip^ toolStrip = gcnew ToolStrip( );
	m_multidocument_base->Controls->Add( toolStrip );
	m_multidocument_base->Controls->SetChildIndex( toolStrip, 1 );
	
	
	ImageList^ particle_image_list		= xray::editor_base::image_loader::load_images("particle_editor_images16x16", "particle_editor.resources", 16, safe_cast<int>(xray::editor::particle_editor_images16x16_count), this->GetType()->Assembly);
	
	// Add render modes button.
	mode_strip_button^  render_mode_button  = gcnew mode_strip_button( );
	render_mode_button->image_offset = 2;
	render_mode_button->ToolTipText = "Render Type";
	List<String^>^ modes = gcnew List<String^>( );
	modes->Add("Normal");
	modes->Add("Dots");
	modes->Add("Sizes");
	render_mode_button->image_list		= particle_image_list;
	render_mode_button->modes = modes; 
	render_mode_button->current_mode = 0;
	toolStrip->Items->Add( render_mode_button );
	render_mode_button->Click += gcnew System::EventHandler(this, &particle_editor::on_render_mode_click);
	
	
	// Add play speed button.
	mode_strip_button^  play_speed_button  = gcnew mode_strip_button( );
	play_speed_button->image_offset = 8;
	List<String^>^ modes3 = gcnew List<String^>( );
	play_speed_button->ToolTipText = "Play Speed";
	
	modes3->Add("1000%");
	modes3->Add("500%");
	modes3->Add("200%");
	modes3->Add("100%");
	modes3->Add("50%");
	modes3->Add("25%");
	modes3->Add("10%");
	modes3->Add("1%");
	
	play_speed_button->image_list		= particle_image_list;
	play_speed_button->modes = modes3;
	play_speed_button->current_mode = 3;
	toolStrip->Items->Add( play_speed_button );
	play_speed_button->Click += gcnew System::EventHandler(this, &particle_editor::on_play_speed_click);
	
	
	// Add play/pause button.
	ToolStripButton^  is_playing_Button  = gcnew ToolStripButton( "Playing" );
	toolStrip->Items->Add( is_playing_Button );
	is_playing_Button->Click += gcnew System::EventHandler(this, &particle_editor::on_playing_click);
	is_playing_Button->Checked = true;
	is_playing_Button->CheckOnClick = true;
	is_playing_Button->Image			= particle_image_list->Images[6];
	is_playing_Button->DisplayStyle		= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	is_playing_Button->ImageScaling		= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	// Add restart button.
	ToolStripButton^  restart_system_Button  = gcnew ToolStripButton( "Restart System" );
	toolStrip->Items->Add( restart_system_Button );
	restart_system_Button->Click			 += gcnew System::EventHandler(this, &particle_editor::on_restart_particle_system);
	restart_system_Button->Image			 = particle_image_list->Images[23];
	restart_system_Button->DisplayStyle		 = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	restart_system_Button->ImageScaling		 = System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
	
	// Add looping button.
	ToolStripButton^  is_looping_Button		 = gcnew ToolStripButton( "Toggle Looping" );
	toolStrip->Items->Add( is_looping_Button );
	is_looping_Button->Click				+= gcnew System::EventHandler(this, &particle_editor::on_looping_particle_system);
	is_looping_Button->Checked				 = true;
	is_looping_Button->CheckOnClick			 = true;
	is_looping_Button->Image				 = particle_image_list->Images[5];
	is_looping_Button->DisplayStyle			 = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	is_looping_Button->ImageScaling			 = System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	// Add toggle post-process button.
	ToolStripButton^  use_post_process_Button	 = gcnew ToolStripButton( "Toggle Post Process" );
	toolStrip->Items->Add( use_post_process_Button );
	use_post_process_Button->Click				+= gcnew System::EventHandler(this, &particle_editor::on_toggle_post_process);
	use_post_process_Button->Checked			 = true;
	use_post_process_Button->CheckOnClick		 = true;
	use_post_process_Button->Image				 = particle_image_list->Images[22];
	use_post_process_Button->DisplayStyle		 = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	use_post_process_Button->ImageScaling		 = System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;

	// Add toggle skybox button.
	ToolStripButton^  use_skybox_Button			= gcnew ToolStripButton( "Toggle Skybox" );
	toolStrip->Items->Add( use_skybox_Button );
	use_skybox_Button->Click					+= gcnew System::EventHandler(this, &particle_editor::on_toggle_skybox);
	use_skybox_Button->Checked					= true;
	use_skybox_Button->CheckOnClick				= true;
	use_skybox_Button->Image					= particle_image_list->Images[24];
	use_skybox_Button->DisplayStyle				= System::Windows::Forms::ToolStripItemDisplayStyle::Image;
	use_skybox_Button->ImageScaling				= System::Windows::Forms::ToolStripItemImageScaling::SizeToFit;
	
	
 	m_input_engine		= gcnew xray::editor_base::input_engine( xray::editor_base::input_keys_holder::ref , gcnew particle_contexts_manager( this ));
 	m_gui_binder		= gcnew xray::editor_base::gui_binder( m_input_engine ); 

	m_gui_binder->set_image_list( "base", shared_images );
	m_gui_binder->set_image_list( "particles", particle_image_list );

	m_form->Deactivate += gcnew EventHandler(this, &particle_editor::on_main_form_deactivate);
	
	xray::editor_base::input_keys_holder::ref->subscribe_on_changed( gcnew xray::editor_base::keys_combination_changed(this,&particle_editor::keys_combination_changed));
	
	m_time_layout_panel->KeyPreview = true;

	m_options_dialog = gcnew xray::editor_base::options_dialog();

	m_options_dialog->register_page("Input", gcnew xray::editor_base::options_page_input(m_input_engine));

	m_view_window->m_collision_tree					= &(*(collision::new_space_partitioning_tree( g_allocator, 1.f, 1024 )));


	active_control									= nullptr;



	m_preview_model_instance	= NEW( render::static_model_ptr )( );
	m_preview_model_root_matrix = NEW( math::float4x4 )( );
	(*m_preview_model_root_matrix).identity();
	m_preview_model_path		= gcnew System::String("");

	on_view_menu_item_select(gcnew Object() , EventArgs::Empty );

	query_create_render_resources();

}

void particle_editor::register_actions( )
{
	//test
	xray::editor_base::action_delegate^ a				= nullptr;
	
	a				= gcnew xray::editor_base::action_delegate("New Document", gcnew xray::editor_base::execute_delegate_managed(this, &particle_editor::on_create_new_file_from_tool_strip ));
	m_input_engine->register_action( a, "Control+N(Global)" );

	a				= gcnew xray::editor_base::action_delegate("Save Document", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::save_active ));
	m_input_engine->register_action( a, "Control+S(Global)" );

	a				= gcnew xray::editor_base::action_delegate("Save All", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::save_all ));
	m_input_engine->register_action( a, "Control+Shift+S(Global)" );

	a				= gcnew xray::editor_base::action_delegate("Exit", gcnew xray::editor_base::execute_delegate_managed( this, &particle_editor::exitToolStripMenuItem_Click ));
	m_input_engine->register_action( a, "Alt+X(Global)" );

	a				= gcnew xray::editor_base::action_delegate("Undo", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::undo ));
	m_input_engine->register_action( a, "Control+Z(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Redo", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::redo ));
	m_input_engine->register_action( a, "Control+Y(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Cut", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::cut ));
	m_input_engine->register_action( a, "Control+X(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Copy", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::copy ));
	m_input_engine->register_action( a, "Control+C(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Paste", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::paste ));
	m_input_engine->register_action( a, "Control+V(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Select All", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::select_all ));
	m_input_engine->register_action( a, "Control+A(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Delete", gcnew xray::editor_base::execute_delegate_managed( m_multidocument_base, &document_editor_base::del ));
	m_input_engine->register_action( a, "Delete(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Disable Nodes", gcnew xray::editor_base::execute_delegate_managed( this, &particle_editor::disable_selected_nodes ));
	m_input_engine->register_action( a, "Control+D(ActiveDocument)" );

	a				= gcnew xray::editor_base::action_delegate("Delete in TreeView", gcnew xray::editor_base::execute_delegate_managed( this, &particle_editor::on_tree_view_remove_document ));
	m_input_engine->register_action( a, "Delete(TreeView)" );
	a				= gcnew xray::editor_base::action_delegate("Copy in TreeView", gcnew xray::editor_base::execute_delegate_managed( this, &particle_editor::on_tree_view_copy_to_clipboard ));
	m_input_engine->register_action( a, "Control+C(TreeView)" );
	a				= gcnew xray::editor_base::action_delegate("Paste in TreeView", gcnew xray::editor_base::execute_delegate_managed( this, &particle_editor::on_tree_view_paste_from_clipboard ));
	a->set_enabled	(gcnew xray::editor_base::enabled_delegate_managed(&process_clipboard));
	m_input_engine->register_action( a, "Control+V(TreeView)" );
	a				= gcnew xray::editor_base::action_delegate("Cut in TreeView", gcnew xray::editor_base::execute_delegate_managed( this, &particle_editor::on_tree_view_cut_to_clipboard ));
	m_input_engine->register_action( a, "Control+X(TreeView)" );
	
	a				= gcnew xray::editor_base::action_delegate("Restart System", gcnew xray::editor_base::execute_delegate_managed(this, &particle_editor::restart_particle_system ));
	m_input_engine->register_action( a, "F5(ActiveDocument)" );
	
	// Maya style mouse actions 
	m_input_engine->register_action( gcnew	xray::editor_base::mouse_action_view_move_xy	( "view maya move xy",	m_view_window ), "Alt+MButton(View)" );
	m_input_engine->register_action( gcnew	xray::editor_base::mouse_action_view_move_z		( "view maya move z",	m_view_window ), "Alt+RButton(View)" );
	m_input_engine->register_action( gcnew	xray::editor_base::mouse_action_view_rotate_around( "view maya rotate",	m_view_window ), "Alt+LButton(View)" );
	
	// Old LE style mouse actions 
	m_input_engine->register_action( gcnew	xray::editor_base::mouse_action_view_rotate		( "view LE rotate",		m_view_window ), "Space+LButton+RButton(View)" );
	m_input_engine->register_action( gcnew	xray::editor_base::mouse_action_view_move_y_reverse( "view LE move y",	m_view_window ), "Space+RButton(View)" );
	m_input_engine->register_action( gcnew	xray::editor_base::mouse_action_view_move_xz_reverse( "view LE move xz",m_view_window ), "Space+LButton(View)" );
	m_input_engine->register_action( gcnew action_focus( "focus", this ), "F(View)");

	m_input_engine->register_action( gcnew	mouse_action_editor_control	( "manipulator drag axis mode",	this, 0, false )	, "LButton" );

	xray::editor_base::editor_control_base^ ec = gcnew xray::editor_base::transform_control_translation(m_transform_control_helper);
	m_input_engine->register_action( gcnew	action_select_active_control( "select translate", this, ec ), "W(View)") ;

	ec = gcnew xray::editor_base::transform_control_rotation(m_transform_control_helper);
	m_input_engine->register_action( gcnew	action_select_active_control( "select rotate", this, ec ), "E(View)") ;

	ec = gcnew xray::editor_base::transform_control_scaling(m_transform_control_helper);
	m_input_engine->register_action( gcnew	action_select_active_control( "select scale", this, ec ), "R(View)") ;

	m_input_engine->register_action( gcnew	action_select_active_control( "select none", this, nullptr ), "Escape(View)") ;
	
//GUI_BINDING

	m_gui_binder->bind(m_multidocument_base->newToolStripMenuItem,			"New Document");
	m_gui_binder->bind(m_multidocument_base->saveToolStripMenuItem,			"Save Document");
	m_gui_binder->bind(m_multidocument_base->saveAllToolStripMenuItem,		"Save All");
	m_gui_binder->bind(m_multidocument_base->exitToolStripMenuItem,			"Exit");
	m_gui_binder->bind(m_multidocument_base->undoToolStripMenuItem,			"Undo");
	m_gui_binder->bind(m_multidocument_base->redoToolStripMenuItem,			"Redo");
	m_gui_binder->bind(m_multidocument_base->cutToolStripMenuItem,			"Cut");
	m_gui_binder->bind(m_multidocument_base->copyToolStripMenuItem,			"Copy");
	m_gui_binder->bind(m_multidocument_base->pasteToolStripMenuItem,		"Paste");
	m_gui_binder->bind(m_multidocument_base->selectAllToolStripMenuItem,	"Select All");
	m_gui_binder->bind(m_multidocument_base->delToolStripMenuItem,			"Delete");

}

void particle_editor::load_particles_library			( )
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &particle_editor::on_particles_library_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/library/particle_configs/particle_source.library",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void particle_editor::on_particles_library_loaded		( xray::resources::queries_result& data )
{
	R_ASSERT(data.is_successful());
	configs::lua_config_ptr config	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	m_particles_library_config		= CRT_NEW(configs::lua_config_value)(config->get_root());

	m_node_style_mgr->load( (*m_particles_library_config)["particle_node_style"] );
	

	m_nodes_panel											= gcnew particle_sources_panel( this );
	m_nodes_panel->load										( *m_particles_library_config );
	m_nodes_panel->Dock										= DockStyle::Fill;
	m_toolbar_panel->Controls->Add							( m_nodes_panel );	

	configs::lua_config_value const& data_source_types_config		= (*m_particles_library_config)["data_source_types"];
	configs::lua_config_value::iterator data_source_types_begin		= data_source_types_config.begin();
	configs::lua_config_value::iterator data_source_types_end		= data_source_types_config.end();

	for( ; data_source_types_begin != data_source_types_end ; ++data_source_types_begin ){

		String^ data_source_type	= gcnew String( data_source_types_begin.key( ));
		String^ panel_name			= gcnew String( ( *data_source_types_begin )[ "panel_name" ] );
		available_data_source_types->Add( data_source_type , panel_name );
	}
	m_data_sources_panel->initialize_panels( m_available_data_source_types );
}



void particle_editor::clear_resources			( )
{
	//Save panel settings
	m_multidocument_base->save_panels			( m_form );

	m_multidocument_base->close_all_documents	( );
	
	m_view_window->clear_resources				( );
	m_view_window->clear_scene					( );
	//m_editor_renderer->destroy_output_window	( &default_window );
	DELETE										( m_output_window );
	m_editor_renderer							= NULL;


	collision::delete_space_partitioning_tree	( m_transform_control_helper->m_collision_tree );

	delete 										m_nodes_panel;
	delete 										m_node_style_mgr;
	delete 										m_options_dialog;
	delete 										m_view_window;
	delete 										m_gui_binder;
	delete 										m_input_engine;
	DELETE										( m_preview_model_instance );
	DELETE										( m_preview_model_root_matrix );
	CRT_DELETE									( m_particles_library_config );
}

void  particle_editor::on_tree_view_expand_node(Object^, TreeViewCancelEventArgs^ e){
	
	tree_node^ t_node = safe_cast<xray::editor::controls::tree_node^>(e->Node);

	if (t_node->first_expand_processed)
		return;
	
	if (t_node->m_node_type != tree_node_type::single_item)
		return;

	particle_tree_node_holder^ node_holder = gcnew particle_tree_node_holder(t_node, this);
	
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(node_holder, &particle_tree_node_holder::on_tree_node_config_loaded), g_allocator);
	xray::resources::query_resource(
		(pcstr)unmanaged_string("resources/"+e->Node->FullPath+".particle").c_str(),
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq,_1),
		g_allocator
		);

	t_node->first_expand_processed = true;
}


IDockContent^ particle_editor::find_dock_content(String^ panel_full_name)
{
	if (panel_full_name == "xray.editor.controls.file_view_panel_base")
		return			m_particles_panel;
	else if (panel_full_name == "xray.editor.controls.item_properties_panel_base")
		return			m_properties_panel;
	else if (panel_full_name == "xray.editor.controls.toolbar_panel_base")
		return			m_toolbar_panel;
	else if (panel_full_name == "xray.editor_base.scene_view_panel" || 
		panel_full_name == "xray.particle_editor.particle_preview_panel") // remove next commit
		return			m_view_window;
	else if (panel_full_name == "xray.particle_editor.particle_data_sources_panel")
		return			m_data_sources_panel;
	else if (panel_full_name == "xray.particle_editor.particle_time_based_layout_panel")
		return			m_time_layout_panel;


	return nullptr;
}

void particle_editor::tree_loaded(Object^ , EventArgs^ )
{
	TreeNodeCollection^ nodes = m_particles_panel->tree_view->root->nodes;
	if(nodes->Count <= 0)
		return;
	
	nodes[0]->Expand();
	nodes[0]->ContextMenuStrip = m_particles_panel->tree_view->ContextMenuStrip;
}

xray::editor::controls::document_base^	particle_editor::on_create_document	()
{
	particle_graph_document^ doc =  gcnew particle_graph_document(m_multidocument_base, this);
	doc->create_root_node(this, EventArgs::Empty);
	
	return doc;
}

//dock panels to sound editor main form
bool			particle_editor::dock_panels					()
{
	bool is_loading_success =  m_multidocument_base->load_panels(m_form);

 	//fill events
 	m_particles_panel->file_double_click		+= gcnew TreeNodeMouseClickEventHandler(this, &particle_editor::on_tree_view_file_double_click);
	m_properties_panel->property_grid_control->auto_update = true;

	return is_loading_success;
}

static void		set_default_name( TreeNode^ dst_node, String^ default_name )
{
	TreeNodeCollection^ nodes		= ( dst_node->Parent != nullptr )? dst_node->Parent->Nodes : dst_node->TreeView->Nodes;
	Int32 index						= 1;
	
	while ( nodes->ContainsKey( default_name+index.ToString( ) ) )
	{
		index++;
	}
	dst_node->Text = default_name+index.ToString();
}

static void		create_particle_file(String^ file_path, particle_editor^ p_editor)
{
	particle_graph_node^ root_node			= gcnew particle_graph_node_emitter(p_editor, image_node_emitter, "particle_entity_root_type", nullptr);
	//root_node->node_text					= "Root";
	root_node->node_config_field			= "Root";
	root_node->particle_entity_type			= "particle_entity_root_type";
	configs::lua_config_value val			= p_editor->particles_library_config["particle_nodes"]["Root"]["properties"];
	root_node->properties->load_properties( &val, "particle_node_properties." + root_node->node_config_field );
	
	
	root_node->movable						= true;
	

	configs::lua_config_ptr	const& config	= configs::create_lua_config(unmanaged_string(file_path).c_str());
	configs::lua_config_value config_item	= config->get_root()["LOD 0"];
	
	config_item.assign_lua_value(*root_node->node_config);

	//root_node->save(config_item, false);
	(config_item)["type"] = "emitter";
	config->save( configs::target_sources );
	delete root_node;
}

// Copy a source directory to a target directory.
void CopyDirectory( String^ SourceDirectory, String^ TargetDirectory , bool overwrite, bool is_move)
{
	IO::DirectoryInfo^ source = gcnew IO::DirectoryInfo( SourceDirectory );
	IO::DirectoryInfo^ target = gcnew IO::DirectoryInfo( TargetDirectory );

	//Determine whether the source directory exists.
	if (  !source->Exists )
		return;

	if (  !target->Exists )
		target->Create();


	//Copy files.
	array<IO::FileInfo^>^sourceFiles = source->GetFiles();
	for ( int i = 0; i < sourceFiles->Length; ++i ){
		if (IO::File::Exists(String::Concat( target->FullName, "/", sourceFiles[ i ]->Name ))){
			if (overwrite)		
				IO::File::Copy( sourceFiles[ i ]->FullName, String::Concat( target->FullName, "/", sourceFiles[ i ]->Name ), true );
		}
		else
			IO::File::Copy( sourceFiles[ i ]->FullName, String::Concat( target->FullName, "/", sourceFiles[ i ]->Name ), true );
		if (is_move)
			IO::File::Delete( sourceFiles[ i ]->FullName );

	}

	//Copy directories.
	array<IO::DirectoryInfo^>^sourceDirectories = source->GetDirectories();
	for ( int j = 0; j < sourceDirectories->Length; ++j ){
		CopyDirectory( sourceDirectories[ j ]->FullName, String::Concat( target->FullName, "/", sourceDirectories[ j ]->Name ), overwrite, is_move);
	}
}


void			particle_editor::on_create_new_file_from_tool_strip	()
{
// 	SaveFileDialog^ dlg = gcnew SaveFileDialog();
// 	dlg->DefaultExt = ".particle";
// 	dlg->InitialDirectory = particle_editor::absolute_particle_resources_path + "particles/";
// 
// 	String^ particle_file_path = gcnew String("");
// 
// 	if(dlg->ShowDialog() == Windows::Forms::DialogResult::OK)
// 	{
// 		particle_file_path = dlg->FileName;//->Substring(dlg->InitialDirectory->Length);
// 		//particle_file_path = particle_file_path->Substring(0,particle_file_path->Length - dlg->DefaultExt->Length);
// 	}
// 	else
// 		return;
// 	
// 	//String^ file_path	= +c_extension;
// 	String^ dir_path	= Path::GetDirectoryName(particle_file_path);
// 
// 	if(!Directory::Exists(dir_path))
// 	{
// 		Directory::CreateDirectory(dir_path);
// 	}
	
	on_tree_view_new_document(gcnew Object(), gcnew EventArgs());
	xray::editor_base::input_keys_holder::ref->reset(this , EventArgs::Empty);
}

void			particle_editor::on_tree_view_paste_from_clipboard	( )
{
	xray::editor::controls::tree_node^ t_node = safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0]);
	if ( t_node->m_node_type == tree_node_type::single_item )
		t_node = safe_cast<xray::editor::controls::tree_node^>(t_node->Parent);
	else if ( t_node->m_node_type == tree_node_type::file_part_item )
		t_node = safe_cast<xray::editor::controls::tree_node^>(t_node->Parent->Parent);

	pcstr string				= xray::os::paste_from_clipboard( *g_allocator );
	// clipboard doesn't contain text information
	if ( !string ) {
		// we could use function xray::os::is_clipboard_empty() to check
		// whether there non-text information and show some message in that case
		return;
	}
	
	bool is_cut		= xray::strings::starts_with( string, "tree_view_clipboard_cut");

	pcstr found				= strrchr( string, '.');
	if ( found )		
		*found++;

	String^ node_to_copy_path = gcnew String(found);
	xray::editor::controls::tree_node^ node_to_copy = m_particles_panel->tree_view->get_node(node_to_copy_path);

	String^ new_node_path = particle_editor::absolute_particle_resources_path+t_node->FullPath+"/"+node_to_copy->Text;
	String^ old_node_path = particle_editor::absolute_particle_resources_path+node_to_copy_path;

	if (node_to_copy->m_node_type == tree_node_type::group_item)
	{
		Windows::Forms::DialogResult result = Windows::Forms::DialogResult::Cancel;
		if (IO::Directory::Exists(new_node_path)){
			String^ message = String::Format("This folder already contains a folder called '" + node_to_copy->Text + "'.If files in the existing folder have the same names as files in the folder you are copying,do you want to replace the existing files?");
			result = Windows::Forms::MessageBox::Show(message, "Paste Directory", MessageBoxButtons::YesNoCancel);
			
			if (result == Windows::Forms::DialogResult::Yes)
			{
				CopyDirectory(	old_node_path,
								new_node_path,
								true,
								is_cut);
			}
			else if(result == Windows::Forms::DialogResult::No)
			{
				CopyDirectory(	old_node_path,
								new_node_path,
								false,
								is_cut);
			}
			else
			{
				return;
			}
		}
		else{
			CopyDirectory(	old_node_path,
							new_node_path,
							true,
							is_cut);
		}
		
 		if ( is_cut ){
 			IO::Directory::Delete(	old_node_path , true);
 			node_to_copy->Remove();
 			xray::os::copy_to_clipboard("");
 		}
 		
		safe_cast<particle_groups_source^>(m_particles_panel->files_source)->refresh(t_node->FullPath + "/" + node_to_copy->Text);			

	}
	else if (node_to_copy->m_node_type == tree_node_type::single_item)
	{
		if (IO::File::Exists(new_node_path+s_file_extension)){
			String^ message = "File '" + new_node_path+"/"+node_to_copy->Text+s_file_extension + "' already exists, are you sure you wish to overwrite it?";
			if (Windows::Forms::MessageBox::Show(message, "Paste File", MessageBoxButtons::YesNo) == Windows::Forms::DialogResult::Yes)
			{
				IO::File::Copy(	old_node_path+s_file_extension,
							new_node_path+s_file_extension,
							true);		
				xray::editor::controls::tree_node^ node = m_particles_panel->tree_view->get_node(t_node->FullPath+"/" + node_to_copy->Text);
				node->nodes->Clear();
				add_context_menu_to_tree_view_node(node->add_node_single("LOD 0", xray::editor_base::particle_tree_icon));
				node->Collapse();
				node->first_expand_processed = false;
			}
			else
			{
				return;
			}
		}
		else{			
			IO::File::Copy(	old_node_path+s_file_extension,
						new_node_path+s_file_extension,
						true);
			xray::editor::controls::tree_node^ added_node = t_node->add_node_single(node_to_copy->Text, xray::editor_base::particle_tree_icon);
			add_context_menu_to_tree_view_node(added_node);
			tree_node^ lod_node = added_node->add_node_file_part("LOD 0");
			add_context_menu_to_tree_view_node(lod_node);
		}
		
		if (is_cut){
			IO::File::Delete(old_node_path+s_file_extension);
			node_to_copy->Remove();
			xray::os::copy_to_clipboard("");
		}					
	}
	t_node->Expand();
	check_for_opened_docs_source_existance(this);
}

void			particle_editor::on_tree_view_copy_to_clipboard		( )
{
	xray::editor::controls::tree_node^ t_node = safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0]);

	String^ copied_node_path = t_node->FullPath;
	
	String^ to_clipboard = "tree_view_clipboard_copy."+copied_node_path;

	xray::os::copy_to_clipboard	( unmanaged_string(to_clipboard).c_str() );
}

void			particle_editor::on_tree_view_cut_to_clipboard		( )
{
	xray::editor::controls::tree_node^ t_node = safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0]);

	String^ copied_node_path = t_node->FullPath;

	String^ to_clipboard = "tree_view_clipboard_cut."+copied_node_path;

	xray::os::copy_to_clipboard	( unmanaged_string(to_clipboard).c_str() );
}

void			particle_editor::on_tree_view_label_edit				(Object^ , NodeLabelEditEventArgs^ e)
{
	Boolean is_file				= safe_cast<xray::editor::controls::tree_node^>(e->Node)->m_node_type == tree_node_type::single_item;
	if(e->Node->Text == " ")
	{
		if(e->Label == nullptr || e->Label->Trim() == "")
		{
			set_default_name		(e->Node, (is_file)?s_default_file_name:s_default_folder_name);
			e->CancelEdit			= true;
		}
		else
			e->Node->Text			= e->Label->Trim();
		
		e->Node->Name			= e->Node->Text;

		if(is_file)
		{
			String^ new_file_path	= (particle_editor::absolute_particle_resources_path + e->Node->FullPath);
			
			if(IO::File::Exists(new_file_path + s_file_extension) || IO::Directory::Exists(new_file_path))
			{
				System::Windows::Forms::MessageBox::Show("File or folder with name \""+e->Label+"\" already exists in the current scope.", "Particle Editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				set_default_name(e->Node, s_default_file_name);
				create_particle_file(IO::Path::GetDirectoryName(new_file_path)->Replace("\\","/")+"/"+e->Node->Text+s_file_extension, this);
				e->CancelEdit	= true;
				e->Node->EnsureVisible();
			}
			else
				create_particle_file(new_file_path+s_file_extension, this);
		}
		else
		{
			String^ new_dir_path	= (particle_editor::absolute_particle_resources_path + e->Node->FullPath);
			
			if(IO::Directory::Exists(new_dir_path) || IO::File::Exists(new_dir_path+s_file_extension))
			{
				System::Windows::Forms::MessageBox::Show("File or folder with name \""+e->Label+"\" already exists in the current scope.", "Particle Editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
				set_default_name(e->Node, s_default_folder_name);
				IO::Directory::CreateDirectory(IO::Path::GetDirectoryName(new_dir_path)+ "/" + e->Node->Text);
				e->CancelEdit	= true;
				e->Node->EnsureVisible();
			}
			else
				IO::Directory::CreateDirectory(new_dir_path);
		}
	}
	else
	{
		if(e->Node->Text == e->Label || e->Label == nullptr)
			return;

		if(is_file)
		{
			String^ new_file_path	= (particle_editor::absolute_particle_resources_path + IO::Path::GetDirectoryName(e->Node->FullPath) + "/" + e->Label->Trim() + s_file_extension);
				
			if(IO::File::Exists(new_file_path) || IO::Directory::Exists(IO::Path::ChangeExtension(new_file_path, "")))
			{
				System::Windows::Forms::MessageBox::Show	("This name already in use!");
				e->CancelEdit		= true;
				return;
			}
			IO::File::Move	(particle_editor::absolute_particle_resources_path + e->Node->FullPath + s_file_extension, new_file_path);
			check_for_opened_docs_names_on_rename(this, e->Node->FullPath, IO::Path::GetDirectoryName(e->Node->FullPath) + "/" + e->Label->Trim());
			e->Node->Text = e->Label;
			e->Node->Name = e->Label;
		}
		else
		{
			String^ new_dir_path	= (particle_editor::absolute_particle_resources_path + IO::Path::GetDirectoryName(e->Node->FullPath) + "/" + e->Label->Trim());
				
			if(IO::Directory::Exists(new_dir_path) || IO::File::Exists(new_dir_path+s_file_extension))
			{
				System::Windows::Forms::MessageBox::Show	("This name already in use!");
				e->CancelEdit		= true;
				return;
			}
			else{
				
				CopyDirectory(particle_editor::absolute_particle_resources_path+e->Node->FullPath, new_dir_path, true, true);
				IO::Directory::Delete(particle_editor::absolute_particle_resources_path+e->Node->FullPath);
				check_for_opened_docs_names_on_rename(this, e->Node->FullPath, IO::Path::GetDirectoryName(e->Node->FullPath) + "/" + e->Label->Trim());
				e->Node->Text = e->Label;
				e->Node->Name = e->Label;
			}
		}
	}
	e->Node->EnsureVisible();
}

void 			particle_editor::on_tree_view_new_document				(Object^ , EventArgs^ )
{
	xray::editor::controls::tree_node^ node = m_particles_panel->tree_view->add_item("particles/ ", xray::editor_base::particle_tree_icon);
	m_particles_panel->tree_view->select_node(node,true,true);
	xray::editor::controls::tree_node^ lod_node = node->add_node_file_part("LOD 0");
	add_context_menu_to_tree_view_node(node);
	add_context_menu_to_tree_view_node(lod_node);
	node->BeginEdit();// Nodes->Add(new_node);
}

void 			particle_editor::on_tree_view_new_folder					(Object^ , EventArgs^ )
{
	xray::editor::controls::tree_node^ node = m_particles_panel->tree_view->add_group("particles/ ", xray::editor_base::folder_closed, xray::editor_base::folder_open);
	m_particles_panel->tree_view->select_node(node,true,true);
	add_context_menu_to_tree_view_node(node);
	node->BeginEdit();
}

void 			particle_editor::on_document_add_new_lod					(Object^ , EventArgs^){
	
	

	xray::editor::controls::tree_node^ node = safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0]);

	if (particle_configs->ContainsKey(node->Parent->FullPath)){
		particle_configs[node->Parent->FullPath]->c_ptr()->save(configs::target_sources);
	}

	particle_tree_node_holder^ node_holder = gcnew particle_tree_node_holder(node, this);

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(node_holder, &particle_tree_node_holder::on_tree_node_new_lod_config_loaded), g_allocator);
	xray::resources::query_resource(
		(pcstr)unmanaged_string("resources/"+node->FullPath+".particle").c_str(),
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq,_1),
		g_allocator
		);
	
}

void			particle_editor::on_document_delete_lod						()
{
	xray::editor::controls::tree_node^ node = safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0]);
	
	String^ message;

	if(node->Parent->FirstNode == node)
		message			= "This operation will remove whole particle system, are you sure?";	
	else if (node->Parent->LastNode == node)
		message			= "Are you sure you want to remove this LOD?";		
	else
		message			= "Are you sure you want to remove this LOD and all lower LODs?";	

	if(Windows::Forms::MessageBox::Show(message, "Items Deleting", MessageBoxButtons::YesNo) == Windows::Forms::DialogResult::Yes)
	{
		if(node->Parent->FirstNode == node)
		{
			IO::File::Delete		(particle_editor::absolute_particle_resources_path+m_particles_panel->tree_view->selected_nodes[0]->Parent->FullPath+s_file_extension);
			m_particles_panel->tree_view->selected_nodes[0]->Parent->Remove();
		}
		else{
			particle_tree_node_holder^ node_holder = gcnew particle_tree_node_holder(node, this);

			query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(node_holder, &particle_tree_node_holder::on_tree_node_delete_lod_config_loaded), g_allocator);
			xray::resources::query_resource(
				(pcstr)unmanaged_string("resources/"+node->Parent->FullPath+".particle").c_str(),
				xray::resources::lua_config_class,
				boost::bind(&query_result_delegate::callback, rq,_1),
				g_allocator
				);
			List<System::Windows::Forms::TreeNode^> nodes_to_delete = gcnew List<TreeNode^>();
			TreeNode^ node_to_delete = m_particles_panel->tree_view->selected_nodes[0];
			while (node_to_delete != m_particles_panel->tree_view->selected_nodes[0]->Parent->LastNode){
				nodes_to_delete.Add(node_to_delete);
				node_to_delete = node_to_delete->NextNode;
			}
			nodes_to_delete.Add(m_particles_panel->tree_view->selected_nodes[0]->Parent->LastNode);
			for each(TreeNode^ n in nodes_to_delete)
			{
				n->Remove();
			}
		}
	}	
}

void 			particle_editor::on_tree_view_folder_new_document			(Object^ , EventArgs^ )
{
	String^ path;

	if(safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0])->m_node_type == tree_node_type::single_item)
		path = (m_particles_panel->tree_view->selected_nodes[0]->Parent == nullptr)
			? " ": m_particles_panel->tree_view->selected_nodes[0]->Parent->FullPath+"/ ";
	else
		path = m_particles_panel->tree_view->selected_nodes[0]->FullPath+"/ ";

	xray::editor::controls::tree_node^ node	= m_particles_panel->tree_view->add_item(path, xray::editor_base::particle_tree_icon);
	node->EnsureVisible	();
	m_particles_panel->tree_view->select_node(node,true,true);
	xray::editor::controls::tree_node^ lod_node = node->add_node_file_part("LOD 0");
	add_context_menu_to_tree_view_node(node);
	add_context_menu_to_tree_view_node(lod_node);
	node->BeginEdit();

}


void 			particle_editor::on_tree_view_remove_document		()
{
	Boolean is_lod		= safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0])->m_node_type == tree_node_type::file_part_item;
	Boolean is_file		= safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0])->m_node_type == tree_node_type::single_item;

	if (is_lod)
	{
		on_document_delete_lod();
		return;
	}

	String^ message;
	if(is_file)
		message			= "Are you sure you want to remove this particle?";
	else
		message			= "Are you sure you want to remove this directory, with all particles?";

	if(Windows::Forms::MessageBox::Show(message, "Items Deleting", MessageBoxButtons::YesNo) == Windows::Forms::DialogResult::Yes)
	{
		if(is_file)
		{
			IO::File::Delete		(particle_editor::absolute_particle_resources_path+m_particles_panel->tree_view->selected_nodes[0]->FullPath+s_file_extension);
			m_particles_panel->tree_view->selected_nodes[0]->Remove();
		}
		else
		{
			IO::Directory::Delete	(particle_editor::absolute_particle_resources_path+m_particles_panel->tree_view->selected_nodes[0]->FullPath, true);
			m_particles_panel->tree_view->selected_nodes[0]->Remove();
		}
	}
	check_for_opened_docs_source_existance(this);
}

void 			particle_editor::on_tree_view_rename_item					(Object^ , EventArgs^ )
{
	m_particles_panel->tree_view->selected_nodes[0]->BeginEdit();
}

void 			particle_editor::on_tree_view_folder_new_folder				(Object^ , EventArgs^ )
{
	String^ path;

	if(safe_cast<xray::editor::controls::tree_node^>(m_particles_panel->tree_view->selected_nodes[0])->m_node_type == tree_node_type::single_item)
		path = (m_particles_panel->tree_view->selected_nodes[0]->Parent == nullptr)
			? " ": m_particles_panel->tree_view->selected_nodes[0]->Parent->FullPath+"/ ";
	else
		path = m_particles_panel->tree_view->selected_nodes[0]->FullPath+"/ ";

	xray::editor::controls::tree_node^ node	= m_particles_panel->tree_view->add_group(path, xray::editor_base::folder_closed, xray::editor_base::folder_open);
	node->EnsureVisible	();
	m_particles_panel->tree_view->select_node(node,true,true);
	add_context_menu_to_tree_view_node(node);
	node->BeginEdit();
}

void particle_editor::close_internal( )
{
	save_settings		( base_registry_key::get() );
	clear_resources ( );
	m_closed		= true;
	m_form->Close	( );
	delete this;
}

void particle_editor::particle_editor_FormClosing(System::Object^, System::Windows::Forms::FormClosingEventArgs^ )
{
	if(m_closed)
		return;

	close_internal();
	//if(e->CloseReason == System::Windows::Forms::CloseReason::UserClosing)
	//{
	//	//manual hide this form
	//	m_form->Hide();
	//	//cancel closing the form
	//	e->Cancel = true;
	//}   
}

void particle_editor::exitToolStripMenuItem_Click(  )
{
	close_internal();
}

void particle_editor::on_tree_view_file_double_click(Object^ , TreeNodeMouseClickEventArgs^ e)
{
	xray::editor::controls::tree_node^ node = safe_cast<xray::editor::controls::tree_node^>(e->Node);
	if(node->m_node_type == tree_node_type::single_item){
	/*	document_base^ doc = */m_multidocument_base->load_document_part(e->Node->FullPath, "LOD 0", true);
	//	doc->Text = node->FullPath+ "/LOD 0";
	}
	else if (node->m_node_type == tree_node_type::file_part_item)
	{
		this->load_document(node);
	}
}

void			particle_editor::load_document					(xray::editor::controls::tree_node^ node)
{
	String^ lod_name = node->FullPath->Substring(node->Parent->FullPath->Length+1);
	safe_cast<particle_graph_document^>(m_multidocument_base->load_document_part(node->Parent->FullPath, lod_name,true));
	//doc->Text = node->FullPath;	
}


void			particle_editor::show_properties				(Object^ obj)
{
	m_properties_panel->property_grid_control->selected_object = obj;
}

void particle_editor::add_context_menu_to_tree_view_node(xray::editor::controls::tree_node^ node)
{
	Forms::ContextMenuStrip^ context_menu = gcnew Forms::ContextMenuStrip();
	if (node->m_node_type == tree_node_type::group_item){
		ToolStripMenuItem^ copy_menu_item		= gcnew ToolStripMenuItem("Copy");
		ToolStripMenuItem^ paste_menu_item		= gcnew ToolStripMenuItem("Paste");
		ToolStripMenuItem^ cut_menu_item		= gcnew ToolStripMenuItem("Cut");
		ToolStripMenuItem^ delete_menu_item		= gcnew ToolStripMenuItem("Delete");

		m_gui_binder->bind(delete_menu_item,	"Delete in TreeView");
		m_gui_binder->bind(copy_menu_item,		"Copy in TreeView");
		m_gui_binder->bind(cut_menu_item,		"Cut in TreeView");
		m_gui_binder->bind(paste_menu_item,		"Paste in TreeView");

		context_menu->Items->AddRange(gcnew array<ToolStripMenuItem^>{
			gcnew ToolStripMenuItem("New Document",		nullptr, gcnew EventHandler(this, &particle_editor::on_tree_view_folder_new_document)),
			gcnew ToolStripMenuItem("New Folder",		nullptr, gcnew EventHandler(this, &particle_editor::on_tree_view_folder_new_folder)),
			gcnew ToolStripMenuItem("Rename",			nullptr, gcnew EventHandler(this, &particle_editor::on_tree_view_rename_item)),
			delete_menu_item,
			copy_menu_item,
			cut_menu_item,			
			paste_menu_item,
		});
	}
	else if(node->m_node_type == tree_node_type::single_item){
		ToolStripMenuItem^ copy_menu_item		= gcnew ToolStripMenuItem("Copy");
		ToolStripMenuItem^ cut_menu_item		= gcnew ToolStripMenuItem("Cut");
		ToolStripMenuItem^ delete_menu_item		= gcnew ToolStripMenuItem("Delete");

		m_gui_binder->bind(delete_menu_item,	"Delete in TreeView");
		m_gui_binder->bind(copy_menu_item,		"Copy in TreeView");
		m_gui_binder->bind(cut_menu_item,		"Cut in TreeView");	

		context_menu->Items->AddRange(gcnew array<ToolStripMenuItem^>{
			gcnew ToolStripMenuItem("New LOD",			nullptr, gcnew EventHandler(this, &particle_editor::on_document_add_new_lod)),
			gcnew ToolStripMenuItem("Rename",			nullptr, gcnew EventHandler(this, &particle_editor::on_tree_view_rename_item)),
			delete_menu_item,
			copy_menu_item,
			cut_menu_item,
		});
	}
	else if(node->m_node_type == tree_node_type::file_part_item){
		ToolStripMenuItem^ delete_menu_item		= gcnew ToolStripMenuItem("Delete");
		m_gui_binder->bind(delete_menu_item,	"Delete in TreeView");
		context_menu->Items->AddRange(gcnew array<ToolStripMenuItem^>{
			delete_menu_item,
		});
	}
	node->ContextMenuStrip = context_menu;
}


particle_graph_node^ particle_editor::deserialize_particle_node_from_config( configs::lua_config_value& node_config ){
	
	//node_config.copy().save_as("d:/ttt");

	images92x25 image_type_id	= images92x25(int(node_config["image_type"]));
	String^ node_type			= gcnew String( node_config["type"] );
	String^ node_entity_type	= gcnew String( node_config["particle_entity_type"] );
	


	String^ node_id				= nullptr;
	if (node_config.value_exists("id"))
		node_id				= gcnew String(node_config["id"]);

	configs::lua_config_value types_config = (particles_library_config)["particle_entity_types"];

	ASSERT(types_config.value_exists( ( pcstr )node_config["particle_entity_type"] ) );
	
	particle_graph_node^ node = nullptr;

	if (node_type == "emitter")
		node = gcnew particle_graph_node_emitter( this ,image_type_id, node_entity_type, node_id );		
	else if (node_type == "event")
		node = gcnew particle_graph_node_event( this ,image_type_id, node_entity_type, node_id );
	else if (node_type == "property")
		node = gcnew particle_graph_node_property( this ,image_type_id, node_entity_type, node_id );
	else 
		node = gcnew particle_graph_node_action( this ,image_type_id, node_entity_type, node_id );
	
	return node;
}

void particle_editor::on_view_menu_item_select(Object^ , EventArgs^ )
{
	toolbar_panel_menu_item->Checked = !m_toolbar_panel->IsHidden;
	file_view_panel_menu_item->Checked = !m_particles_panel->IsHidden;
	properties_panel_menu_item->Checked = !m_properties_panel->IsHidden;
	time_layout_menu_item->Checked = !m_time_layout_panel->IsHidden;
	data_sources_menu_item->Checked = !m_data_sources_panel->IsHidden;
	preview_panel_menu_item->Checked = !m_view_window->IsHidden;
}

void particle_editor::on_toolbar_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(toolbar_panel_menu_item->Checked))
		m_toolbar_panel->Hide();
	else
		m_toolbar_panel->Show(m_multidocument_base->main_dock_panel);
}

void particle_editor::on_file_view_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(file_view_panel_menu_item->Checked))
		m_particles_panel->Hide();
	else
		m_particles_panel->Show(m_multidocument_base->main_dock_panel);
}

void particle_editor::on_properties_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(properties_panel_menu_item->Checked))
		m_properties_panel->Hide();
	else
		m_properties_panel->Show(m_multidocument_base->main_dock_panel);
}

void particle_editor::on_time_layout_menu_select(Object^ , EventArgs^ )
{
	if(!(time_layout_menu_item->Checked))
		m_time_layout_panel->Hide();
	else
		m_time_layout_panel->Show(m_multidocument_base->main_dock_panel);
}

void particle_editor::on_data_sources_menu_select(Object^ , EventArgs^ )
{
	if(!(data_sources_menu_item->Checked))
		m_data_sources_panel->Hide();
	else
		m_data_sources_panel->Show(m_multidocument_base->main_dock_panel);
}

void particle_editor::on_preview_panel_menu_select(Object^ , EventArgs^ )
{
	if(!(preview_panel_menu_item->Checked))
		m_view_window->Hide();
	else
		m_view_window->Show(m_multidocument_base->main_dock_panel);
}
                  
void particle_editor::on_main_form_deactivate		(Object^ , EventArgs^ )
{
	keys_combination_changed("None");
}

void particle_editor::keys_combination_changed( System::String^ combination )
{
	System::String^ status	= "Keys: ["+combination+"]";
	toolStripStatusLabel0->Text = status;
}

void particle_editor::disable_selected_nodes	()
{
	if (m_multidocument_base->active_document != nullptr){
		particle_hypergraph^ p_hypergraph = safe_cast<particle_hypergraph^>(safe_cast<particle_graph_document^>(m_multidocument_base->active_document)->hypergraph);
		if (p_hypergraph->selected_nodes->Count > 0)
			p_hypergraph->comm_engine->run(gcnew particle_graph_commands::disable_group_nodes(p_hypergraph, p_hypergraph->selected_nodes));
	}
}

using namespace Microsoft::Win32;

void particle_editor::load_settings( RegistryKey^ product_key ) 
{
	RegistryKey^ editor_key		= xray::base_registry_key::get_sub_key(product_key, m_name);

	System::String^ selected_preview_model = dynamic_cast<System::String^>( editor_key->GetValue( "selected_preview_model" ) );
	
	if ( selected_preview_model != nullptr && selected_preview_model != "" )
	{
		setup_preview_model( selected_preview_model );
	}

	m_view_window->load_settings	( editor_key );
	m_input_engine->load			( editor_key );

}

void particle_editor::save_settings( RegistryKey^ product_key ) 
{
 	RegistryKey^ editor_key			= xray::base_registry_key::get_sub_key(product_key, m_name);

	editor_key->SetValue			( "selected_preview_model", m_preview_model_path );
	m_view_window->save_settings	( editor_key );
	m_input_engine->save			( );
}

void particle_editor::show_options			( Object^ , EventArgs^ )
{
	m_input_engine->stop		();
	m_options_dialog->ShowDialog			(m_form);
	m_input_engine->resume	();
}

void particle_editor::reset_transform ( )
{
	transform_helper->m_changed = true;
}



void particle_editor::setup_preview_model	(System::String^ library_item_path)
{

	m_preview_model_path = library_item_path;

	if ( m_preview_model_path != ""){

		query_result_delegate* q = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &particle_editor::on_preview_model_loaded), g_allocator);

		resources::query_resource	(
			unmanaged_string(m_preview_model_path).c_str(),
			resources::static_model_instance_class,
			boost::bind(&query_result_delegate::callback, q, _1),
			g_allocator
			);
	}
	else
	{
		m_editor_renderer->scene().remove_model( *m_scene, (*m_preview_model_instance)->m_render_model );
		DELETE( m_preview_model_instance );
		m_preview_model_instance	= NEW( render::static_model_ptr )( );
	}
}

void particle_editor::on_preview_model_loaded( resources::queries_result& data )
{
	R_ASSERT( data.is_successful() );

	if ( (*m_preview_model_instance).c_ptr() != NULL )
		m_editor_renderer->scene().remove_model( *m_scene, (*m_preview_model_instance)->m_render_model );

	*m_preview_model_instance	= static_cast_resource_ptr<render::static_model_ptr>(data[0].get_unmanaged_resource());
	
	m_editor_renderer->scene().add_model( *m_scene, (*m_preview_model_instance)->m_render_model, *m_preview_model_root_matrix );
}

void particle_editor::on_add_preview_model_menu_item_click	(Object^ , EventArgs^ )
{
	System::Windows::Forms::OpenFileDialog^ Dialog = gcnew System::Windows::Forms::OpenFileDialog();
	Dialog->DefaultExt = L"model";
	Dialog->Filter = L"Model files|*.model";
	Dialog->RestoreDirectory = true;
	Dialog->Title = L"Open model";
	Dialog->InitialDirectory = System::IO::Path::GetFullPath("../../resources/exported/models");
	Dialog->Multiselect = false;

	if ( Dialog->ShowDialog( m_form )==System::Windows::Forms::DialogResult::OK )
	{
		u32 index = Dialog->FileName->IndexOf("\\models\\")+8;
		System::String^ path = Dialog->FileName->Remove(0, index)->Replace("\\", "/");
		path = path->Remove(path->Length-6);
		setup_preview_model( path );
	}
}

void particle_editor::on_clear_preview_model_menu_item_click	(Object^ , EventArgs^ )
{
	setup_preview_model( "" );
}

void particle_editor::on_scene_view_mode_click		( Object^ sender , EventArgs^ )
{
	int mode = safe_cast<mode_strip_button^>( sender )->current_mode;
	
	switch (mode)
	{
		case 0:	m_editor_renderer->scene().set_view_mode( *m_scene_view, render::wireframe_two_sided_view_mode );	break;
		case 1:	m_editor_renderer->scene().set_view_mode( *m_scene_view, render::unlit_view_mode );					break;
		case 2:	m_editor_renderer->scene().set_view_mode( *m_scene_view, render::lit_view_mode );					break;
		case 3:	m_editor_renderer->scene().set_view_mode( *m_scene_view, render::texture_density_view_mode );		break;
		case 4:	m_editor_renderer->scene().set_view_mode( *m_scene_view, render::shader_complexity_view_mode );		break;
	}
}

void particle_editor::on_render_mode_click			( Object^ sender , EventArgs^ )
{
	int mode = safe_cast<mode_strip_button^>( sender )->current_mode;
	m_editor_renderer->scene().set_particles_render_mode( 0, xray::particle::enum_particle_render_mode( mode ) );
}

void particle_editor::on_play_speed_click			( Object^ sender , EventArgs^ )
{
	int mode = safe_cast<mode_strip_button^>( sender )->current_mode;
	
	switch (mode)
	{
		case 0: m_current_play_speed = 10.0f; break;
		case 1:	m_current_play_speed = 5.00f; break;
		case 2:	m_current_play_speed = 2.00f; break;
		case 3: m_current_play_speed = 1.00f; break;
		case 4:	m_current_play_speed = 0.50f; break;
		case 5:	m_current_play_speed = 0.25f; break;
		case 6:	m_current_play_speed = 0.10f; break;
		case 7:	m_current_play_speed = 0.01f; break;
		default: return;
	}

	m_editor_renderer->scene().set_slomo( *m_scene, m_current_play_speed );
}

void particle_editor::on_playing_click			( Object^, EventArgs^ )
{
	m_is_playing = !m_is_playing;
	m_editor_renderer->scene().set_slomo( *m_scene, m_is_playing ? m_current_play_speed : 0.0f );
}

void particle_editor::on_restart_particle_system	( Object^, EventArgs^ )
{
	restart_particle_system();
}

void particle_editor::restart_particle_system()
{
	if (m_multidocument_base->active_document)
	{
		particle_graph_document^ doc = safe_cast<particle_graph_document^>(m_multidocument_base->active_document);
		doc->on_restart_particle_system();
	}
}

void particle_editor::on_looping_particle_system	( Object^, EventArgs^ )
{
	m_is_looping_particle_system = !m_is_looping_particle_system;
	
	if (m_multidocument_base->active_document)
	{
		particle_graph_document^ doc = safe_cast<particle_graph_document^>(m_multidocument_base->active_document);
		doc->set_looping_preview_particle_system( m_is_looping_particle_system );		
	}
}

void particle_editor::on_toggle_post_process	( Object^, EventArgs^ )
{
	m_is_use_post_process = !m_is_use_post_process;
	
	m_editor_renderer->scene().enable_post_process( *m_scene_view, m_is_use_post_process);
}

void particle_editor::on_toggle_skybox	( Object^, EventArgs^ )
{
	m_is_use_skybox = !m_is_use_skybox;

	m_editor_renderer->scene().toggle_render_stage( render::skybox_render_stage, m_is_use_skybox);
}

}//namespace particle_editor
}//namespace xray
