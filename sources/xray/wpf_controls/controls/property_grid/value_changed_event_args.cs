////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_grid
{
	public class value_changed_event_args:EventArgs
	{
		public value_changed_event_args(property prop, Object old_value)
		{
			changed_property = prop;
			this.old_value = old_value;
		}

		public property		changed_property	{ get; private set; }
		public Object		old_value			{ get; private set; }
		public Object		new_value			{ get { return changed_property.value; } }
	}
}
