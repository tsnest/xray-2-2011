////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node_emitter.h"
#include "images/images92x25.h"
#include "images/images104x8.h"
#include "particle_graph_node_collection.h"


namespace xray {
namespace particle_editor {
Boolean				is_drag_available(particle_graph_node^ parent_node, particle_graph_node^ node)
{
	if 	(parent_node->is_root_node())
		return (node->is_emitter_node() || node->is_particle_group_node());
	else if (parent_node->is_particle_group_node() && node->is_emitter_node())
		return true;
	else if(!parent_node->is_particle_group_node() && !node->is_emitter_node())		
		return true;			
	return false;
}

Boolean particle_graph_node_emitter::can_accept_node(particle_graph_node^ node){
	return super::can_accept_node(node) && is_drag_available(this, node) ;
}

void particle_graph_node_emitter::in_constructor(){
	m_rect_images =	xray::editor_base::image_loader::load_images(
																"images104x8",
																"particle_editor.resources",
																104,
																safe_cast<int>(images104x8_count),
																this->GetType()->Assembly
																);	
	super::in_constructor();
}

void draw_nodes_container(Graphics^ g, int first_child_id, int last_child_id, particle_graph_node_collection^ children, ImageList^ images_list){
	particle_graph_node^ first_child = children->nodes_dictionary[children[first_child_id]];
	particle_graph_node^ last_child = children->nodes_dictionary[children[last_child_id-1]];

	Point point_top = Point(first_child->Location.X-6,first_child->Location.Y-8);
	Point point_bottom = Point(first_child->Location.X-6,last_child->Location.Y+25);
	Point ulCorner = Point(first_child->Location.X-6,first_child->Location.Y);
	Point urCorner = Point(first_child->Location.X+98,first_child->Location.Y);
	Point llCorner = Point(first_child->Location.X-6,last_child->Location.Y+28);
	array<Point>^ destPara = {ulCorner,urCorner,llCorner};
	g->DrawImage(images_list->Images[image_container_top], point_top);
	g->DrawImage(images_list->Images[image_container_bottom], point_bottom);
	g->DrawImage(images_list->Images[image_container_middle], destPara);
}


void particle_graph_node_emitter::OnPaint			(PaintEventArgs^ e)
{
	if(m_parent != nullptr)
	{
		ImageList^ particle_node_images = safe_cast<particle_graph_node_style_mgr^>(parent_hypergraph->nodes_style_manager)->images;
		Image^ node_image = particle_node_images->Images[image_type];

		e->Graphics->DrawImage(node_image, Drawing::Point(0,0));

		if (m_highlighted)
			e->Graphics->DrawImage(particle_node_images->Images[image_node_highlited], Drawing::Point(0,0));
		if (enabled && children->Count > 0 && parent_node != nullptr && parent_node->active_node == this)
		{
			Graphics^ g = parent_hypergraph->CreateGraphics();
			if (children->last_property_id>0)
			{
				int first_property_id = 0;
				int last_property_id = children->last_property_id;
				draw_nodes_container(g, first_property_id,last_property_id,children,m_rect_images);					
			}
			if (children->last_action_id>0)
			{
				int first_action_id = children->last_property_id;
				int last_action_id	= children->last_property_id + children->last_action_id;
				draw_nodes_container(g, first_action_id,last_action_id,children,m_rect_images);
			}
			if (children->last_event_id>0)
			{
				int first_event_id	= children->last_property_id + children->last_action_id;
				int last_event_id	= children->last_property_id + children->last_action_id + children->last_event_id;
				draw_nodes_container(g, first_event_id,last_event_id,children,m_rect_images);
			}
			if (is_particle_group_node())
				draw_nodes_container(g, 0, children->Count, children, m_rect_images);
		}
		if (is_root_node() && children->Count > 0)
		{
			Graphics^ g = parent_hypergraph->CreateGraphics();
			draw_nodes_container(g, 0, children->Count, children, m_rect_images);
		}
	}
	else {
		particle_editor^ editor = safe_cast<particle_editor^>(m_parent_editor);
		e->Graphics->DrawImage(editor->node_style_mgr->images->Images[image_type], Drawing::Point(0,0));		
	}
	super::OnPaint(e);
}
} // namespace particle_editor {
} // namespace xray
