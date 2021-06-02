////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_COLLECTION_H_INCLUDED
#define PARTICLE_GRAPH_NODE_COLLECTION_H_INCLUDED

using namespace System;

namespace xray {
	namespace editor {
		ref class particle_graph_node;
		ref class particle_graph_node_collection : public Collections::Generic::List<particle_graph_node^>
{
	typedef Collections::Generic::List<particle_graph_node^>	super;
public:
	particle_graph_node_collection(particle_graph_node^ owner);

	virtual void	Insert			(Int32 i, particle_graph_node^ node) new;
	virtual void	Add				(particle_graph_node^ node) new;
	virtual void	Remove			(particle_graph_node^ node) new;
			bool	MoveToIndex		(particle_graph_node^ node, int index);

private:
	particle_graph_node^			m_owner;
	int								m_last_property_id;
	int								m_last_action_id;
	int								m_last_event_id;
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
}; // class particle_graph_node_collection
	}
}
#endif // #ifndef PARTICLE_GRAPH_NODE_COLLECTION_H_INCLUDED