////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_ACTION_H_INCLUDED
#define PARTICLE_GRAPH_NODE_ACTION_H_INCLUDED
#include "particle_graph_node.h"

namespace xray {
	namespace editor {

		public ref class particle_graph_node_action : public particle_graph_node{
			typedef particle_graph_node super;
		public:
			particle_graph_node_action(xray::editor_base::images92x25 image_type_id, particle_editor^ editor):
			  particle_graph_node(editor)	
			
			{
				m_image_type		= image_type_id;
			}

		private:

		protected:
			virtual void					OnPaint			(PaintEventArgs^ e) override;
		
		public:
			virtual	Boolean					can_accept_node(particle_graph_node^ ) override {return false;};

		}; // class particle_graph_node_emitter
	} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_GRAPH_NODE_ACTION_H_INCLUDED