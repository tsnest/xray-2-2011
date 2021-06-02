////////////////////////////////////////////////////////////////////////////
//	Created		: 17.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_PART_COLLISION_H_INCLUDED
#define OBJECT_PATROL_GRAPH_PART_COLLISION_H_INCLUDED

#include <xray/collision/collision_object.h>

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph_part;

		class object_patrol_graph_part_collision: public collision::collision_object
		{
		private:
			typedef collision::collision_object	super;
		
		public:
			object_patrol_graph_part_collision		( collision::geometry_instance* geometry, object_patrol_graph_part^ obj );

		public:
			virtual bool				touch			( ) const;
			object_patrol_graph_part^	get_owner		( ) const { return m_owner; }

		private:
			gcroot<object_patrol_graph_part^>		m_owner;

		}; // class object_patrol_graph_part_collision
	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_PART_COLLISION_H_INCLUDED