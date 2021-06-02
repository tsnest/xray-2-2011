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
#include <xray/editor/base/managed_delegate.h>
#include <xray/strings_stream.h>
#include "particle_sources_panel.h"

using namespace xray::editor;
using xray::editor::particle_graph_document;
using xray::editor::particle_links_manager;
using xray::editor::particle_graph_node;
using xray::editor::particle_node_connection_editor;
using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::connection_editor;
using xray::editor::controls::hypergraph::connection_point;

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
	m_hypergraph->nodes_style_manager = safe_cast<particle_editor^>(m_editor->ParentForm)->node_style_mgr;
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
}

void						particle_graph_document::create_root_node		(Object^ sender, EventArgs^ e)
{
	XRAY_UNREFERENCED_PARAMETERS(sender, e);
	particle_graph_node^ root_node =  make_node();
	
	configs::lua_config_value* node_config = parent_editor->node_sources_panel->particle_nodes_config;
	root_node->node_config_field = "Root";
	root_node->properties = gcnew particle_node_properties(root_node, "emitter", "Root");
	root_node->load_node_properties((*node_config)["Root"]["properties"]);

	root_node->position = Drawing::Point(m_hypergraph->display_rectangle.X + 10,m_hypergraph->display_rectangle.Y + 10);
	m_hypergraph->append_node(root_node);
		
	root_node->Text = "Root";	
	root_node->movable		= true;
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
void						particle_graph_document::add_connection			(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination)
{
	links^ lnks = gcnew links();
	lnks->Add(create_connection(source, destination));
	m_hypergraph->add_connection(lnks);
}

void						particle_graph_document::add_connection			(links^ lnks)
{
	m_hypergraph->add_connection(lnks);
}	

particle_graph_node^		particle_graph_document::make_node				()
{
	particle_graph_node^ n = gcnew particle_graph_node_emitter(xray::editor_base::image_node_emitter, parent_editor);
	m_hypergraph->nodes_style_manager->assign_style(n, "particle_node");
	return n;
}

void						particle_graph_document::save					()
{
	if(!m_has_name)
	{
		SaveFileDialog^ dlg = gcnew SaveFileDialog();
		dlg->DefaultExt = c_extension;
		dlg->InitialDirectory = particle_editor::absolute_particle_resources_path + "particles\\";

		if(dlg->ShowDialog() == Windows::Forms::DialogResult::OK)
		{
			Name = dlg->FileName->Substring(dlg->InitialDirectory->Length);
			Name = Name->Substring(0,Name->Length - c_extension->Length);
			Text = Name;
			m_has_name = true;
		}
		else
			return;
	}




	//create correct path
	String^ file_path	= particle_editor::absolute_particle_resources_path+"particles\\"+Name+c_extension;
	String^ dir_path	= Path::GetDirectoryName(file_path);

	if(!Directory::Exists(dir_path))
	{
		Directory::CreateDirectory(dir_path);
	}
	
	//open config file and get root of it
	configs::lua_config_ptr	const&		config				= configs::create_lua_config(unmanaged_string(file_path).c_str());
	configs::lua_config_value			config_item			= config->get_root()["root"];

	m_hypergraph->root_node->save(config_item);

	configs::lua_config_value			config_item_ds		= config->get_root()["data_sources"];
	m_data_sources->save(config_item_ds);
	config->save();
	m_hypergraph->save();
	
}

void						particle_graph_document::load					()
{
		String^ m_file_path = (particle_editor::particle_resources_path+"/particles/"+Name+c_extension);

		query_result_delegate* rq = XRAY_NEW_IMPL(g_allocator, query_result_delegate)(gcnew query_result_delegate::Delegate(this, &particle_graph_document::on_document_loaded));
		xray::resources::query_resource(
			unmanaged_string(m_file_path).c_str(),
			xray::resources::config_lua_class,
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

void						particle_graph_document::on_document_loaded		(xray::resources::queries_result& data)
{
	ASSERT(!data.is_failed());
	
	//get object of config reader
	configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	
	if (config->get_root().value_exists("data_sources")){
		m_data_sources->load(config->get_root()["data_sources"]);
	}
	
	if(config->get_root().value_exists("root"))
	{
		//get config root
		configs::lua_config_value const& config_root = config->get_root()["root"];

		m_hypergraph->SuspendLayout();
		m_hypergraph->root_node->load(config_root);
		m_hypergraph->ResumeLayout(false);
		m_hypergraph->clear_command_stack();
		m_hypergraph->save();
	}

	is_saved = true;
	m_has_name = true;
}

void particle_graph_document::on_form_activated(System::Object^ sender, System::EventArgs^ e)
{
	super::on_form_activated(sender, e);
	if (this->Visible){
		this->m_data_sources->add_sources_to_panel(m_particle_editor->data_sources_panel);	
		on_selection_changed();
	}
}

void particle_graph_document::on_selection_changed	()
{
	if(m_hypergraph->selected_nodes != nullptr && m_hypergraph->selected_nodes->Count>0){
		safe_cast<particle_editor^>(m_editor->Parent)->time_based_layout_panel->show_node(safe_cast<particle_graph_node^>(m_hypergraph->selected_nodes[0]));
		m_editor->show_properties( safe_cast<particle_graph_node^>(m_hypergraph->selected_nodes[0])->prop_holder->container());
	}
	else{
		safe_cast<particle_editor^>(m_editor->Parent)->time_based_layout_panel->clear_panel();	
		m_editor->show_properties(nullptr);
	}
}

void						particle_graph_document::copy					(Boolean is_cut)
{
	if(m_hypergraph->selected_nodes->Count > 0)
	{
		configs::lua_config_ptr cfg = configs::create_lua_config( );
		configs::lua_config_value val = (*cfg)["root"];

		Int32 index = -1;

		for each(particle_graph_node^ node in m_hypergraph->selected_nodes)
			safe_cast<particle_graph_node^>(node)->save(val[++index]);

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

void						particle_graph_document::paste					()
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
		m_hypergraph->comm_engine->run(gcnew particle_graph_commands::add_group(m_hypergraph, config,safe_cast<particle_graph_node^>(m_hypergraph->selected_nodes[0])));
	}
	
}

void						particle_graph_document::del						()
{
	if(m_hypergraph->selected_nodes->Count > 0)
	{
		m_cuted_nodes = nullptr;
		nodes^ nodes_to_remove = gcnew nodes();
		for each(particle_graph_node^ node in m_hypergraph->selected_nodes)
		{
			if(node->parent_graph != nullptr && node == node->parent_graph->root_node){
				MessageBox::Show("You can not remove ROOT node!", 
					"Information", MessageBoxButtons::OK, MessageBoxIcon::Information);
				continue;
			}

				//node->owner->children->Remove(node);
				nodes_to_remove->Add(node);
		}
		if (nodes_to_remove->Count != 0){
			m_hypergraph->remove_group(nodes_to_remove);
 			m_hypergraph->selected_links->Clear();
 			m_hypergraph->set_selected_node((nodes^)nullptr);
		}
	}

}

String^						particle_graph_document::get_document_name		(String^ full_name)
{
	return full_name->Substring(full_name->IndexOf("/")+1);
}

void particle_graph_document::on_form_closing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e){
	super::on_form_closing(sender, e);
	parent_editor->data_sources_panel->cc_page->Controls->Clear();
	parent_editor->data_sources_panel->fc_page->Controls->Clear();
	delete m_hypergraph;
	delete m_data_sources;
}

