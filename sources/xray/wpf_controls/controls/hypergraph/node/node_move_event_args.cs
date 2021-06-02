////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.hypergraph
{
	public class node_move_args
	{
		public node_move_args( node node, Vector offset )
		{
			node_id			= node.id;
			new_position	= node.position;
			old_position	= new_position + offset;
		}
		public		String		node_id;
		public		Point		new_position;
		public		Point		old_position;
	}

	public class node_move_event_args: EventArgs
	{
		public node_move_event_args( List<node_move_args> args )
		{
			this.args			= args;
		}

		public	List<node_move_args> args;
	}
}
