using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls
{
	public class property_grid_value_changed_event_args:EventArgs
	{
		public property_grid_value_changed_event_args(property_grid_property prop, Object old_value)
		{
			changed_property = prop;
			this.old_value = old_value;
		}

		public property_grid_property		changed_property	{ get; private set; }
		public Object		old_value			{ get; private set; }
		public Object		new_value			{ get { return changed_property.value; } }
	}
}
