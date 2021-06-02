////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_sources_panel.h"
#include "particle_source_nodes_container.h"
#include "particle_graph_node_emitter.h"
#include "particle_graph_node_action.h"
#include "particle_graph_node_event.h"
#include "particle_graph_node_property.h"
#include <xray/editor/base/images/images92x25.h>
#include <xray/editor/base/managed_delegate.h>

using namespace System::Collections::Generic;

namespace xray {
namespace editor {

void particle_sources_panel::in_constructor(particle_editor^ editor){
	m_editor					= editor;
	m_particle_nodes_containers = gcnew particle_nodes_containers();
	
	this->AutoScroll			= true;

	load_tools_panel();
}
static void add_new_control(particle_sources_panel^ owner, Control^ control){
	if (owner->Controls->Count == 0)
		control->Location = Drawing::Point(0,0);
	else
		control->Location = Drawing::Point(0,owner->Controls[owner->Controls->Count-1]->Location.Y + owner->Controls[owner->Controls->Count-1]->Size.Height);
	owner->Controls->Add(control);
}
particle_source_nodes_container^ particle_sources_panel::add_container(String^ container_name){
	particle_source_nodes_container^ new_container = gcnew particle_source_nodes_container(container_name);
	new_container->Size = System::Drawing::Size(this->Size.Width, 15);
	m_particle_nodes_containers->Add(new_container);
	add_new_control(this, new_container);
	new_container->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &particle_sources_panel::on_container_click);
	return new_container;
}

particle_graph_node^ particle_sources_panel::add_node_to_container(particle_source_nodes_container^ container, String^ node_type, String^ node_name, xray::editor_base::images92x25 image_type_id){
	particle_graph_node^	node		= nullptr;
	
	if(node_type == "emitter")
		node	= gcnew particle_graph_node_emitter(image_type_id,m_editor);
	else if (node_type == "event")
		node	= gcnew particle_graph_node_event(image_type_id,m_editor);
	else if (node_type == "property")
		node	= gcnew particle_graph_node_property(image_type_id,m_editor);
	else
		node	= gcnew particle_graph_node_action(image_type_id,m_editor);

	node->is_on_graph					= false;
	node->properties					= gcnew particle_node_properties(node, node_type->ToLower(), node_name);
	m_editor->node_style_mgr->assign_style		(node, "particle_node");
	node->Size							= node->size;
	node->position						= Drawing::Point(20, 5);
	node->Text							= node_name;
	node->Anchor						= AnchorStyles::Top|AnchorStyles::Left;
	add_new_control						(this, node);

	container->particle_source_nodes->Add(node);
	
	return node;
}

System::Void particle_sources_panel::on_container_click(System::Object^  sender, System::Windows::Forms::MouseEventArgs^ ){
	particle_source_nodes_container^ container  = safe_cast<particle_source_nodes_container^>(sender);
	int offset = 0;
	for each (particle_graph_node^ node in container->particle_source_nodes){
		node->Visible = !node->Visible;
		if (node->Visible == false)
			offset = offset - node->size.Height;
		else
			offset = offset + node->size.Height;
	}
	if (offset<0){
		offset = offset + 2;
		container->enabled = false;
		container->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
	}
	else{
		offset = offset - 2;	
		container->enabled = true;
		container->BackColor = System::Drawing::SystemColors::GradientActiveCaption;	
	}

	int last_node_index = this->Controls->IndexOf(container->particle_source_nodes[container->particle_source_nodes->Count-1]);
	for (int i = last_node_index; i < this->Controls->Count;i++){
		Control^ contr = this->Controls[i];
		contr->Location = Drawing::Point(contr->Location.X, contr->Location.Y + offset);	
	}
	this->Invalidate(false);
}


void particle_sources_panel::load_tools_panel(){
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &particle_sources_panel::on_toolbar_panel_loaded));
	xray::resources::query_resource(
		"resources/library/particle_configs/particle_source.library",
		xray::resources::config_lua_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

}
void particle_sources_panel::on_toolbar_panel_loaded(xray::resources::queries_result& data){
	R_ASSERT(!data.is_failed());
	configs::lua_config_ptr config	= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	
	m_particle_nodes_config			= NEW(configs::lua_config_value)(config->get_root()["particle_nodes"]);
	
	if(config->get_root().value_exists("particle_toolbar")){
		configs::lua_config_value const& toolbar_config = config->get_root()["particle_toolbar"];
		configs::lua_config_value::iterator begin	= toolbar_config.begin();
		configs::lua_config_value::iterator end		= toolbar_config.end();

		for(;begin != end;++begin){
			
			String^ container_name = gcnew String((*begin)["name"]);

			particle_source_nodes_container^ container = add_container(container_name);

			if ((*begin).value_exists("children")){
				configs::lua_config_value children						= (*begin)["children"];
				configs::lua_config_value::iterator begin_children		= children.begin();
				configs::lua_config_value::iterator end_children		= children.end();

				for(;begin_children != end_children;++begin_children){
					pcstr node_config_str = (pcstr)(*begin_children);

					configs::lua_config_value node_config = (*m_particle_nodes_config)[node_config_str];

					String^ node_name = gcnew String(node_config["name"]);
					String^ node_type = gcnew String(node_config["type"]);
					xray::editor_base::images92x25 image_type = xray::editor_base::images92x25(int(node_config["image_type"]));
					particle_graph_node^ node = add_node_to_container	(container ,node_type,	node_name, image_type);
					
					node->node_config_field = gcnew String(node_config_str);

					if (node_config.value_exists("properties"))
						node->load_node_properties(node_config["properties"]);
				}
			}
		}
	}
}


} // namespace editor
} // namespace xray