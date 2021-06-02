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
#include "images/images92x25.h"

using namespace System::Collections::Generic;
using namespace xray::particle;

namespace xray {
	namespace particle_editor {

void particle_sources_panel::in_constructor(particle_editor^ editor){
	m_editor					= editor;
	m_particle_nodes_containers = gcnew particle_nodes_containers();
	this->AutoScroll			= true;
}

static void add_new_control(particle_sources_panel^ owner, Control^ control){
	if (owner->Controls->Count == 0)
		control->Location = Drawing::Point(0,0);
	else
	{
		int location_y = owner->Controls[owner->Controls->Count-1]->Location.Y + owner->Controls[owner->Controls->Count-1]->Size.Height;
		control->Location = Drawing::Point( 0, location_y );
	}
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

particle_graph_node^ particle_sources_panel::add_node_to_container(	particle_source_nodes_container^ container,
																	String^ node_name,
																	configs::lua_config_value& node_config)
{
	particle_graph_node^	node		= m_editor->deserialize_particle_node_from_config(node_config );
	
	(*node->node_config)["type"]		= node_config["type"];

	node->is_on_graph					= false;
		
	m_editor->node_style_mgr->assign_style		(node, "particle_node");
	node->Size							= node->size;
	node->position						= Drawing::Point(20, 5);
	node->node_text						= node_name;
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


void particle_sources_panel::load( configs::lua_config_value config )
{
	if(config.value_exists("particle_toolbar")){
		configs::lua_config_value const& toolbar_config = config["particle_toolbar"];
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

					configs::lua_config_value node_config = config["particle_nodes"][node_config_str];

					String^ node_name = gcnew String(node_config["name"]);
					
				
					particle_graph_node^ node = add_node_to_container	(container , node_name, node_config);
					
					node->node_config->add_super_table("library/particle_configs/particle_source.library", unmanaged_string( "particle_entity_types." + node->particle_entity_type ).c_str() );
					
					node->node_config_field = gcnew String(node_config_str);	

					(*node->node_config)["type"].assign_lua_value(node_config["type"]);

					if (node_config.value_exists("properties")){
						configs::lua_config_value val = node_config["properties"];					
						node->properties->load_properties( &val, "particle_node_properties." + node->node_config_field );
					}
				}
			}
		}
	}
}


} // namespace particle_editor
} // namespace xray