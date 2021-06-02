////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PATROL_GRAPH_ACTION_H_INCLUDED
#define OBJECT_PATROL_GRAPH_ACTION_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		ref class object_patrol_graph_action abstract : editor_base::action_single
		{
			typedef editor_base::action_single	super;

		public:
			object_patrol_graph_action				( String^ name );
			virtual bool			enabled			( )	override;

		}; // class object_patrol_graph_action

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PATROL_GRAPH_ACTION_H_INCLUDED