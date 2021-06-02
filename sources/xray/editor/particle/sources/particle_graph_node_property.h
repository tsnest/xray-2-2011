////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_PROPERTY_H_INCLUDED
#define PARTICLE_GRAPH_NODE_PROPERTY_H_INCLUDED

#include "particle_graph_node.h"

namespace xray {
	namespace particle_editor {

		public ref class particle_graph_node_property : public particle_graph_node{
			typedef particle_graph_node super;
		public:
			particle_graph_node_property( particle_editor^ editor, images92x25 image_type_id, String^ entity_type,  String^ node_id ):
			particle_graph_node(editor)	
			{
				image_type			= image_type_id;
				particle_entity_type	= entity_type;
				id						= node_id;
			}

		protected:
			virtual		void				OnPaint			(PaintEventArgs^ e) override;

		public:
			virtual		Boolean				can_accept_node(particle_graph_node^ ) override {return false;};

		}; // class particle_graph_node_property

	} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_GRAPH_NODE_PROPERTY_H_INCLUDED