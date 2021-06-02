////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.hypergraph
{
	public class can_link_event_args: EventArgs
	{
		public can_link_event_args( link_point source, link_point destination )
		{
			this.source			= source;
			this.destination	= destination;
			can_link	= false;
		}

		public		link_point	destination;
		public		link_point	source;
		public		Boolean		can_link;
	}
}
