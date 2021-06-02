////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_COLLECTION_H_INCLUDED
#define PARTICLE_GRAPH_NODE_COLLECTION_H_INCLUDED

using namespace System;

namespace xray {
	namespace particle_editor {
		ref class particle_graph_node;
		ref class particle_graph_node_collection : public Collections::Generic::List<String^>
{
	typedef Collections::Generic::List<String^>	super;
	typedef Collections::Generic::Dictionary<String^, particle_graph_node^> particle_nodes_dictionary;
public:
	particle_graph_node_collection(particle_graph_node^ owner);

			void	Insert			(Int32 i, particle_graph_node^ node);
			void	Add				(particle_graph_node^ node) ;
			void	Remove			(particle_graph_node^ node) ;
			bool	MoveToIndex		(particle_graph_node^ node, int index);

private:
	particle_graph_node^			m_owner;
	int								m_last_property_id;
	int								m_last_action_id;
	int								m_last_event_id;

	particle_nodes_dictionary^		m_nodes_dictionary;

public:
	property int 					last_property_id{
		int							get(){return m_last_property_id;}
	}
	property int 					last_action_id{
		int							get(){return m_last_action_id;}
	}
	property int 					last_event_id{
		int							get(){return m_last_event_id;}
	}
	property particle_nodes_dictionary^ nodes_dictionary{
		particle_nodes_dictionary^		get(){return m_nodes_dictionary;}
	}
}; // class particle_graph_node_collection
	} // namespace particle_editor
} // namespace xray
#endif // #ifndef PARTICLE_GRAPH_NODE_COLLECTION_H_INCLUDED