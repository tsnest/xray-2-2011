////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_document.h"
#include "particle_editor.h"
#include "particle_graph_node_style.h"
#include "particle_graph_node.h"
#include "particle_graph_node_emitter.h"
#include "particle_node_connection_editor.h"
#include "particle_links_manager.h"
#include "particle_node_properties.h"
#include "particle_graph_node_collection.h"
#include "add_delete_group_nodes.h"
#include <xray/strings_stream.h>
#include "particle_sources_panel.h"
#include "particle_system_config_wrapper.h"
#include "particle_data_sources_panel.h"
#include "particle_time_based_layout_panel.h"
#include "transform_control_helper.h"
#include "property_visualizers.h"


using namespace xray::editor;
using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::connection_editor;
using xray::editor::controls::hypergraph::connection_point;

namespace xray{
namespace particle_editor{

void particle_graph_document::in_constructor()
{
	m_data_sources = gcnew particle_data_sources_holder(this);

	m_hypergraph = (gcnew particle_hypergraph(this));
	SuspendLayout();

	// m_hypergraph
	m_hypergraph->AllowDrop = true;
	m_hypergraph->Dock = DockStyle::Fill;
	m_hypergraph->BackColor = System::Drawing::Color::DarkGray;
	m_hypergraph->BorderStyle = System::Windows::Forms::BorderStyle::None;
	m_hypergraph->connection_editor = gcnew particle_node_connection_editor(this);
	m_hypergraph->current_view_mode = xray::editor::controls::hypergraph::view_mode::full;
	m_hypergraph->focused_link = nullptr;
	m_hypergraph->focused_node = nullptr;
	m_hypergraph->Location = System::Drawing::Point(2, 2);
	m_hypergraph->max_scale = 3;
	m_hypergraph->min_scale = 0.3F;
	m_hypergraph->move_mode = xray::editor::controls::hypergraph::nodes_movability::by_nodes_movability;
	m_hypergraph->link_style = xray::editor::controls::hypergraph::nodes_link_style::line;
	m_hypergraph->Name = L"m_hypergraph";
	m_hypergraph->node_size_brief = System::Drawing::Size(80, 40);
	m_hypergraph->nodes_style_manager = m_particle_editor->node_style_mgr;
	m_hypergraph->scale = 1;
	m_hypergraph->TabIndex = 0;
	m_hypergraph->visibility_mode = xray::editor::controls::hypergraph::nodes_visibility::all;
	m_hypergraph->on_selection_changed += gcnew xray::editor::controls::hypergraph::selection_changed(this, &particle_graph_document::on_selection_changed);
	m_links_manager = gcnew	particle_links_manager(m_hypergraph);
	m_hypergraph->links_manager	= m_links_manager;
	m_hypergraph->clear();
	
	safe_cast<particle_graph_node_style_mgr^>(m_hypergraph->nodes_style_manager)->library_loaded += gcnew EventHandler(this, &particle_graph_document::create_root_node);
	
	

	this->DockPanel =  (gcnew WeifenLuo::WinFormsUI::Docking::DockPanel());
	this->DockPanel->ActiveAutoHideContent = nullptr;
	this->DockPanel->Dock = System::Windows::Forms::DockStyle::Fill;
	this->DockPanel->DockBackColor = System::Drawing::SystemColors::Control;
	this->DockPanel->DockLeftPortion = 0.5;
	this->DockPanel->DocumentStyle = WeifenLuo::WinFormsUI::Docking::DocumentStyle::DockingWindow;
	this->DockPanel->Location = System::Drawing::Point(0, 24);
	this->DockPanel->Name = L"m_main_dock_panel";
	this->DockPanel->Size = System::Drawing::Size(292, 249);

	this->Controls->Add						(this->DockPanel);
	this->DockPanel->Controls->Add			(m_hypergraph);

	
	
	ResumeLayout(false);
	source_path = System::IO::Path::GetDirectoryName(Application::ExecutablePath)+"\\..\\..\\resources\\particles\\configs\\";
	extension = ".pcl";

	m_lod_name = "LOD 0";


	m_particle_system_ptr = CRT_NEW(xray::particle::particle_system*)();
	(*m_particle_system_ptr) = 0;
	m_particle_system_root_matrix	= NEW(math::float4x4)();
	(*m_particle_system_root_matrix).identity();
}

void particle_graph_document::create_root_node		(Object^ , EventArgs^ )
{
	configs::lua_config_value particle_system_initialize_config = configs::create_lua_config()->get_root()["root"];
	parent_editor->editor_renderer->add_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr,particle_system_initialize_config); 

	(*m_particle_system_root_matrix).identity();
}

void particle_graph_document::root_matrix_modified( cli::array<System::String^>^ )
{
	parent_editor->editor_renderer->update_preview_particle_system( m_particle_editor->scene(), m_particle_system_ptr, *m_particle_system_root_matrix );
}

void particle_graph_document::node_property_modified( cli::array<System::String^>^ names )
{
	m_focused_node->notify_render_on_property_changed( names );
}

controls::hypergraph::link^ particle_graph_document::create_connection		(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination)
{
	particle_graph_node^ src	= safe_cast<particle_graph_node^>(source);
	particle_graph_node^ dst	= safe_cast<particle_graph_node^>(destination);
	xray::editor::controls::hypergraph::connection_point^ src_pt = src->get_connection_point("child");
	xray::editor::controls::hypergraph::connection_point^ dst_pt = dst->get_connection_point("parent");
	if(src_pt==nullptr)
		src_pt = src->add_connection_point(particle_graph_node::typeid, "child");

	if(dst_pt==nullptr)
		dst_pt = dst->add_connection_point(particle_graph_node::typeid, "parent");

	xray::editor::controls::hypergraph::link^ new_link = gcnew xray::editor::controls::hypergraph::link(src_pt, dst_pt);
	
	return new_link;	
}
void particle_graph_document::add_connection			(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination)
{
	links^ lnks = gcnew links();
	lnks->Add(create_connection(source, destination));
}

void particle_graph_document::add_connection			(links^ lnks)
{
	XRAY_UNREFERENCED_PARAMETERS(lnks);
}	

particle_graph_node^		particle_graph_document::make_node				()
{
	particle_graph_node^ n = gcnew particle_graph_node_emitter(parent_editor, image_node_emitter, "particle_entity_root_type", nullptr);
	m_hypergraph->nodes_style_manager->assign_style(n, "particle_node");
	return n;
}

void particle_graph_document::save					()
{
	//create correct path
	String^ file_path	= particle_editor::absolute_particle_resources_path+"particles/"+source_path+c_extension;
	String^ dir_path	= System::IO::Path::GetDirectoryName(file_path);

	if(!System::IO::Directory::Exists(dir_path))
	{
		System::IO::Directory::CreateDirectory(dir_path);
	}
	
	configs::lua_config_ptr	const&		config				= parent_editor->particle_configs["particles/" + source_path]->c_ptr();

	config->save(configs::target_sources);
	for each (document_base^ doc in parent_editor->multidocument_base->opened_documents){
		if (doc->source_path == source_path){
			particle_graph_document^ particle_document = safe_cast<particle_graph_document^>(doc);
			particle_document->hypergraph->save();
		}
	}
	m_hypergraph->save();
	
}

void particle_graph_document::load_partialy					(String^ part_path)
{
		this->lod_name = part_path;

		String^ m_file_path = (particle_editor::particle_resources_path+"/particles/"+source_path+c_extension);

		if (parent_editor->particle_configs->ContainsKey("particles/" + source_path)){
			load_from_config(parent_editor->particle_configs["particles/" + source_path]->c_ptr());
			return;
		}

		query_result_delegate* rq = XRAY_NEW_IMPL(g_allocator, query_result_delegate)(gcnew query_result_delegate::Delegate(this, &particle_graph_document::on_document_loaded), g_allocator);
		xray::resources::query_resource(
			unmanaged_string(m_file_path).c_str(),
			xray::resources::lua_config_class,
			boost::bind(&query_result_delegate::callback, rq, _1),
			g_allocator
		);

}

void particle_graph_document::undo()
{
	m_hypergraph->undo();	
}

void particle_graph_document::redo()
{
	m_hypergraph->redo();	
}

void particle_graph_document::load_from_config(xray::configs::lua_config_ptr config)
{
	if ( !config->get_root( ).value_exists( "data_sources" ) ){
		config->get_root( )["data_sources"].create_table();
	}

	m_data_sources->deserialize_from_config( config->get_root( )[ "data_sources" ] );

	if ( !parent_editor->particle_configs->ContainsKey("particles/" + source_path ) )
		parent_editor->particle_configs->Add("particles/" + source_path, gcnew particle_system_config_wrapper( config ));
	
	if(config->get_root().value_exists(unmanaged_string(lod_name).c_str()))
	{

		if ((*config).value_exists("documents_needs_to_reload"))
		{
			if ((*config)["documents_needs_to_reload"].value_exists(unmanaged_string(lod_name).c_str()))
				(*config)["documents_needs_to_reload"].erase(unmanaged_string(lod_name).c_str());
		}

		//get config root
		configs::lua_config_value config_root = config->get_root()[unmanaged_string(lod_name).c_str()];
		
		m_hypergraph->SuspendLayout();

		// root node_initialization

		particle_graph_node^ root_node	= parent_editor->deserialize_particle_node_from_config( config_root );
		m_hypergraph->nodes_style_manager->assign_style(root_node, "particle_node");
		m_hypergraph->append_node(root_node);	
		root_node->movable				= true;
		root_node->position				= Drawing::Point(m_hypergraph->display_rectangle.X + 10,m_hypergraph->display_rectangle.Y + 10);
		root_node->load					( config_root );
		
		m_hypergraph->ResumeLayout(false);
		m_hypergraph->clear_command_stack();
		m_hypergraph->save();

		configs::lua_config_value particle_system_initialize_config = configs::create_lua_config()->get_root()["root"];
		configs::lua_config_value initialize_config = particle_system_initialize_config["initialize_config"].create_table();
		m_hypergraph->root_node->get_initialize_config(initialize_config);
		particle_system_initialize_config["data_sources"].assign_lua_value( (*config)["data_sources"].copy( ) );

		//particle_system_initialize_config.save_as("d:/ttt");

		parent_editor->editor_renderer->add_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr,particle_system_initialize_config); 

		parent_editor->editor_renderer->show_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr);
	}
	is_saved = true;
	m_has_name = true;
}

void particle_graph_document::on_document_loaded		(xray::resources::queries_result& data)
{
	ASSERT(data.is_successful());
	
	//get object of config reader
	configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	
	load_from_config( config );
	
}

void particle_graph_document::reload_document()
{
	m_particle_editor->show_properties(nullptr);

	m_hypergraph->clear();

	create_root_node(gcnew Object(), gcnew EventArgs());
	load_from_config(parent_editor->particle_configs["particles/" + source_path]->c_ptr());

}

void particle_graph_document::on_form_activated(System::Object^ sender, System::EventArgs^ e)
{
	if (this->Visible && parent_editor->multidocument_base->active_document != this){
		super::on_form_activated(sender, e);
		if (parent_editor->particle_configs->ContainsKey("particles/" + source_path)){
			configs::lua_config_ptr config = parent_editor->particle_configs["particles/" + source_path]->c_ptr();
			if (config->value_exists("documents_needs_to_reload"))
			{
				if ((*config)["documents_needs_to_reload"].value_exists(unmanaged_string(lod_name).c_str()))
					reload_document();
			}
		}

		parent_editor->data_sources_panel->show_data_sources( data_sources->data_sources_dictionary );	
		on_selection_changed();
		parent_editor->editor_renderer->show_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr);
	}
	
}

void particle_graph_document::on_selection_changed	()
{
	if(m_hypergraph->selected_nodes != nullptr && m_hypergraph->selected_nodes->Count>0){
		particle_graph_node^ n = safe_cast<particle_graph_node^>(m_hypergraph->selected_nodes[0]);
		m_particle_editor->time_based_layout_panel->show_node(n);
		m_particle_editor->show_properties(n->properties->container);
		setup_editors_and_visualizers(n);
	}
	else{
		m_particle_editor->time_based_layout_panel->clear_panel();	
		m_particle_editor->show_properties(nullptr);
		setup_editors_and_visualizers(nullptr);
	}
}

void particle_graph_document::setup_editors_and_visualizers( particle_graph_node^ n )
{
	m_focused_node = n;
	parent_editor->transform_helper->m_object = nullptr;
	parent_editor->transform_helper->m_changed = true;
	m_prop_visualizers.Clear();
	if(n==nullptr)
	{
		return;
	}if(n->is_root_node())
	{ // global preview transform
		parent_editor->transform_helper->m_object = gcnew editor_base::transform_value_object( m_particle_system_root_matrix, gcnew editor_base::value_modified(this, &particle_graph_document::root_matrix_modified), true );
		parent_editor->transform_helper->m_changed = true;
	}

	{
		System::String^ base_cfg_field			= n->node_config_field;
		configs::lua_config_value	cfg_library	= n->parent_editor->particles_library_config;
		configs::lua_config_value	cfg_node	= *(n->properties->config);
		configs::lua_config_value	cfg_props	= (cfg_library)["particle_node_properties"][unmanaged_string(base_cfg_field).c_str()];

		configs::lua_config_iterator	it		= cfg_props["editors"].begin();
		configs::lua_config_iterator	it_e	= cfg_props["editors"].end();
		
		for( ;it!=it_e; ++it)
		{
			configs::lua_config_value current = *it;
			pcstr editor_type					= current["type"];
			if(0==strings::compare(editor_type, "transform"))
			{
				pcstr pos_field = current["pos"];
				pcstr rot_field = current["rot"];
				pcstr scale_field = current["scale"];
				configs::lua_config_value	t_position	= cfg_node[pos_field];
				configs::lua_config_value	t_rotation	= cfg_node[rot_field];
				configs::lua_config_value	t_scale		= cfg_node[scale_field];
				cli::array<System::String^>^ names_list = gcnew cli::array<System::String^>( 3 );//{ pos_fileld, rot_field, scale_field };
				names_list[0] = gcnew System::String(pos_field);
				names_list[1] = gcnew System::String(rot_field);
				names_list[2] = gcnew System::String(scale_field);
				parent_editor->transform_helper->m_object = gcnew editor_base::transform_lua_config_object( t_position, t_rotation, t_scale, m_particle_system_root_matrix, gcnew editor_base::value_modified(this, &particle_graph_document::node_property_modified),names_list );
				parent_editor->transform_helper->m_changed = true;
			}else
			{
				UNREACHABLE_CODE();
			}
		}// for each in editors

//		if( (bool)cfg_node["DrawGizmo"] )
		{
		it		= cfg_props["visualizers"].begin();
		it_e	= cfg_props["visualizers"].end();
		
		for( ;it!=it_e; ++it)
		{
			configs::lua_config_value current = *it;
			pcstr editor_type					= current["type"];
			editor_base::property_visualizer^	v = nullptr;
			if(0==strings::compare(editor_type, "domain"))
			{
				pcstr property_name_fileld = current["property"];

				configs::lua_config_value	t_domain	= (strings::length(property_name_fileld)>0) ? cfg_node[property_name_fileld] : cfg_node;
				v = gcnew property_visualizer_domain	( t_domain );
			}else
			{
				UNREACHABLE_CODE();
			}
			if(v!=nullptr)
				m_prop_visualizers.Add		( v );
		}// for each in visualizers
		} // true==["DrawGizmo"]

	}
}

void particle_graph_document::draw( )
{
	for each (editor_base::property_visualizer^ v in m_prop_visualizers)
	{
		v->draw( *m_particle_system_root_matrix, parent_editor->editor_renderer->debug(), parent_editor->scene().c_ptr() );
	}
}

void particle_graph_document::copy(Boolean is_cut)
{
	if(m_hypergraph->selected_nodes->Count > 0)
	{
		configs::lua_config_ptr cfg = configs::create_lua_config( );
		configs::lua_config_value val = (*cfg)["root"];

		Int32 index = -1;

		for each(particle_graph_node^ node in m_hypergraph->selected_nodes)
		{
			if (!is_cut){
				safe_cast<particle_graph_node^>(node)->save_with_only_default_inheritance(val[++index]);
			}
			else
				safe_cast<particle_graph_node^>(node)->save(val[++index]);
		}

		xray::strings::stream stream(g_allocator);
		cfg->save					( stream );
		stream.append				( "%c", 0 );

		xray::os::copy_to_clipboard	( (pcstr)stream.get_buffer() );

		if(is_cut)
		{
			m_hypergraph->remove_group(m_hypergraph->selected_nodes);
		}		
	}
}

void particle_graph_document::paste					()
{
	if(m_hypergraph->selected_nodes->Count > 0)
	{
		pstr string				= xray::os::paste_from_clipboard( *g_allocator );
		// clipboard doesn't contain text information
		if ( !string ) {
			// we could use function xray::os::is_clipboard_empty() to check
			// whether there non-text information and show some message in that case
			return;
		}

		configs::lua_config_ptr config = xray::configs::create_lua_config_from_string( string );
		FREE					( string );
		
		// clipboard does contain text information
		if ( !config ) {
			// but there are no valid lua script in clipboard
			// we could show message about this
			return;
		}

		if ( config->empty() ) {
			// we could show message about this
			return;
		}
		m_hypergraph->comm_engine->run(gcnew particle_graph_commands::add_group(m_hypergraph, config,safe_cast<particle_graph_node^>(m_hypergraph->selected_nodes[0])->id));
	}
	
}

void particle_graph_document::del						()
{
	if(m_hypergraph->selected_nodes->Count > 0)
	{
		m_cuted_nodes = nullptr;
		nodes^ nodes_to_remove = gcnew nodes();
		for each(particle_graph_node^ node in m_hypergraph->selected_nodes)
		{
			if (node->parent_lod_name != lod_name)
				continue;

			if(node->parent_graph != nullptr && node == node->parent_graph->root_node){
				MessageBox::Show("You can't delete ROOT node!", 
					"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
				continue;
			}
			nodes_to_remove->Add(node);
		}
		if (nodes_to_remove->Count != 0){
			m_hypergraph->remove_group(nodes_to_remove);
 			m_hypergraph->selected_links->Clear();
 			m_hypergraph->set_selected_node((nodes^)nullptr);
			on_selection_changed();
		}
	}

}

String^ particle_graph_document::get_document_name		(String^ full_name)
{
	return full_name->Substring(full_name->IndexOf("/")+1);
}

void particle_graph_document::on_form_closing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e){
	m_particle_editor->show_properties(nullptr);
	super::on_form_closing(sender, e);
	
	configs::lua_config_ptr config = parent_editor->particle_configs["particles/" + source_path]->c_ptr();
	if (config->value_exists("documents_needs_to_reload"))
	{
		if ((*config)["documents_needs_to_reload"].value_exists(unmanaged_string(lod_name).c_str()))
			(*config).erase(unmanaged_string(lod_name).c_str());
	}

	bool links_to_cfg_exists = false;
	for each (document_base^ doc in parent_editor->multidocument_base->opened_documents){
		if (doc->source_path == source_path){
			links_to_cfg_exists = true;
			break;
		}
	}
	if (!links_to_cfg_exists)
		parent_editor->particle_configs->Remove("particles/" + source_path);

	delete m_hypergraph;
	delete m_data_sources;
	
	parent_editor->transform_helper->m_object = nullptr;
	parent_editor->transform_helper->m_changed = true;
	parent_editor->editor_renderer->remove_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr);
}

void particle_graph_document::on_restart_particle_system()
{
	parent_editor->editor_renderer->restart_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr);
}

void particle_graph_document::set_looping_preview_particle_system( bool looping )
{
	parent_editor->editor_renderer->set_looping_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr, looping);
}

u32 particle_graph_document::get_num_info_fields()
{
	return parent_editor->editor_renderer->get_num_preview_particle_system_emitters(m_particle_editor->scene(), m_particle_system_ptr);
}

void particle_graph_document::gather_statistics(xray::particle::preview_particle_emitter_info* out_buffer)
{
	parent_editor->editor_renderer->gather_preview_particle_system_statistics(m_particle_editor->scene(), m_particle_system_ptr, out_buffer);
}

void particle_graph_document::particle_system_property_changed(particle_graph_node^ node, xray::configs::lua_config_value& value){
 	configs::lua_config_value outgoing	= configs::create_lua_config()->get_root()["incoming"];
 	outgoing["command_type"]			= "changed";
 	outgoing["id"]						= unmanaged_string(node->id).c_str();
 	outgoing["changed_values"].assign_lua_value(value);
	//outgoing.save_as("d:/sss");
	parent_editor->editor_renderer->update_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr, outgoing);

}

void particle_graph_document::particle_system_node_added(particle_graph_node^ node, xray::configs::lua_config_value& value){
 	configs::lua_config_value outgoing	= configs::create_lua_config()->get_root()["incoming"];
 	outgoing["command_type"]			= "added";
 	outgoing.add_super_table( "library/particle_configs/particle_source.library", unmanaged_string( "particle_entity_types." + node->particle_entity_type ).c_str());
 	outgoing["id"]				= unmanaged_string(node->id).c_str();
 	outgoing["parent_id"]		= unmanaged_string(node->parent_node->id).c_str();
 	outgoing["initial_values"].assign_lua_value(value);
 	parent_editor->editor_renderer->update_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr, outgoing);
}

void particle_graph_document::particle_system_node_removed(particle_graph_node^ node){
 	configs::lua_config_value outgoing	= configs::create_lua_config()->get_root()["incoming"];
 	outgoing["command_type"]			= "removed";
 	outgoing["id"]						= unmanaged_string(node->id).c_str();
 	parent_editor->editor_renderer->update_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr, outgoing);
}

void particle_graph_document::particle_system_property_order_changed(String^ node_id0, String^ node_id1)
{
 	configs::lua_config_value outgoing	= configs::create_lua_config()->get_root()["incoming"];
 	outgoing["command_type"]			= "order_changed";
 	outgoing["id0"]						= unmanaged_string(node_id0).c_str();
 	outgoing["id1"]						= unmanaged_string(node_id1).c_str();
 	parent_editor->editor_renderer->update_preview_particle_system(m_particle_editor->scene(), m_particle_system_ptr, outgoing);
}


}
}
