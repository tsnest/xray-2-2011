////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_graph_node_event.h"
#include <xray/editor/base/images/images92x25.h>
#include <xray/editor/base/images/images104x8.h>
#include "particle_graph_node_collection.h"
#include <xray/editor/base/images_loading.h>


namespace xray {
namespace editor {

extern void draw_nodes_container(Graphics^ g, int first_child_id, int last_child_id, particle_graph_node_collection^ children, ImageList^ images_list);

void particle_graph_node_event::in_constructor(){
	m_rect_images =	 xray::editor_base::image_loader::load_images("images104x8", 104, safe_cast<int>(xray::editor_base::images104x8_count), this->GetType());	
	super::in_constructor();
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
		Image^ node_image = particle_node_images->Images[m_image_type];

		e->Graphics->DrawImage(node_image, Drawing::Point(0,0));

		if (m_highlighted)
			e->Graphics->DrawImage(particle_node_images->Images[xray::editor_base::image_node_highlited], Drawing::Point(0,0));

		if (children->Count > 0 && enabled() && parent_node->active_node == this){
			Graphics^ g = parent_hypergraph->CreateGraphics();
			draw_nodes_container(g, 0,children->Count, children,m_rect_images);			
		}

	}
	else {
		particle_editor^ editor = safe_cast<particle_editor^>(safe_cast<DockContent^>(ParentForm)->DockPanel->Parent->Parent);
		e->Graphics->DrawImage(editor->node_style_mgr->images->Images[m_image_type], Drawing::Point(0,0));		
	}

	super::OnPaint(e);
}



} // namespace editor
} // namespace xray