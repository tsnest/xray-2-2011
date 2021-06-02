////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.hypergraph
{
	public class links_event_args: EventArgs
	{
		public		links_event_args( List<String> link_ids )
		{
			this.link_ids = link_ids;
		}

		public		List<String>		link_ids;
	}
}
