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
namespace editor {

	public ref class particle_graph_node_emitter :public particle_graph_node{
	typedef particle_graph_node super;
	public:
		particle_graph_node_emitter(xray::editor_base::images92x25 image_type_id, particle_editor^ editor):
		particle_graph_node(editor)		
		{
			m_image_type		= image_type_id;
			
			in_constructor		( );
			time_layout_props	= gcnew particle_time_based_layout_props();
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

} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_GRAPH_NODE_EMITTER_H_INCLUDED