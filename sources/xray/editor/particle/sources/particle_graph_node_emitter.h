////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_EMITTER_H_INCLUDED
#define PARTICLE_GRAPH_NODE_EMITTER_H_INCLUDED
#include "particle_graph_node.h"
#include "particle_time_based_layout_props.h"

namespace xray {
namespace particle_editor {

	public ref class particle_graph_node_emitter :public particle_graph_node{
	typedef particle_graph_node super;
	public:
		particle_graph_node_emitter( particle_editor^ editor, images92x25 image_type_id, String^ entity_type,  String^ node_id ):
		  particle_graph_node(editor)	
		  {
				image_type			= image_type_id;
				particle_entity_type	= entity_type;
				id						= node_id;
				in_constructor		( );
				time_layout_props	= gcnew particle_time_based_layout_props();
				(*m_node_config)["children"].create_table();
				(*m_node_config)["children_order"].create_table();
			}

	public:
		property particle_time_based_layout_props^ time_layout_props;

	private:
		void							in_constructor() new;

	private:
		ImageList^						m_rect_images;

	protected:
		virtual		void				OnPaint			(PaintEventArgs^ e) override;

	public:
		virtual		Boolean				can_accept_node(particle_graph_node^ node) override;


}; // class particle_graph_node_emitter

} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_GRAPH_NODE_EMITTER_H_INCLUDED