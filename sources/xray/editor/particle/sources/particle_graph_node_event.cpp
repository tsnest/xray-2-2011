////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node_event.h"
#include "images/images92x25.h"
#include "images/images104x8.h"
#include "particle_graph_node_collection.h"


namespace xray {
namespace particle_editor {

extern void draw_nodes_container(Graphics^ g,int first_child_id,int last_child_id, particle_graph_node_collection^ children,ImageList^ images_list);

void particle_graph_node_event::in_constructor(){
	m_rect_images =	 xray::editor_base::image_loader::load_images(	"images104x8",
																	"particle_editor.resources",
																	104, safe_cast<int>(images104x8_count),
																	this->GetType()->Assembly);	
	super::in_constructor();
	(*m_node_config)["children"].create_table();
	(*m_node_config)["children_order"].create_table();
}

Boolean particle_graph_node_event::can_accept_node	(particle_graph_node^ node)
{
	return super::can_accept_node(node) && (node->is_emitter_node() || node->is_particle_group_node());
}

void particle_graph_node_event::OnPaint				(PaintEventArgs^ e)
{
	if(m_parent != nullptr)
	{
		ImageList^ particle_node_images = safe_cast<particle_graph_node_style_mgr^>(parent_hypergraph->nodes_style_manager)->images;
		Image^ node_image = particle_node_images->Images[image_type];

		e->Graphics->DrawImage(node_image, Drawing::Point(0,0));

		if (m_highlighted)
			e->Graphics->DrawImage(particle_node_images->Images[image_node_highlited], Drawing::Point(0,0));

		if (children->Count > 0 && enabled && parent_node->active_node == this){
			Graphics^ g = parent_hypergraph->CreateGraphics();
			draw_nodes_container(g, 0,children->Count, children,m_rect_images);			
		}

	}
	else {
		particle_editor^ editor = safe_cast<particle_editor^>(m_parent_editor);
		e->Graphics->DrawImage(editor->node_style_mgr->images->Images[image_type], Drawing::Point(0,0));		
	}

	super::OnPaint(e);
}



} // namespace particle_editor
} // namespace xray