////////////////////////////////////////////////////////////////////////////
//	Created		: 20.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.hypergraph
{
	public class menu_event_args: EventArgs
	{
		public menu_event_args( MenuItem item, String item_name, String item_short_name )
		{
			this.menu_item			= item;
			this.item_name			= item_name;
			this.item_short_name	= item_short_name;
		}

		public MenuItem menu_item;
		public String	item_name;
		public String	item_short_name;
	}
}