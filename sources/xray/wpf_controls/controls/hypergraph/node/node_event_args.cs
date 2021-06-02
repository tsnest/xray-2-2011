////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.hypergraph
{
	public class node_event_args: EventArgs
	{
		public node_event_args( String node_id )
		{
			this.node_id = node_id;
		}

		public		String		node_id;
	}
}
