////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.hypergraph
{
	public class selection_event_args: EventArgs
	{
		public selection_event_args( List<String> selected_node_ids, List<link_id> selected_link_ids )
		{
			this.selected_link_ids = selected_link_ids;
			this.selected_node_ids = selected_node_ids;
		}

		public		List<String>		selected_node_ids;
		public		List<link_id>		selected_link_ids;

	}
}
