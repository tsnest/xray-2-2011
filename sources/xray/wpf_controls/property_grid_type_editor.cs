using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class property_grid_type_editor: property_grid_editor
	{
		public property_grid_type_editor()
		{
		}

		public property_grid_type_editor( Type edited_type, System.Windows.DataTemplate editor_template )
		{
			this.edited_type		= edited_type;
			this.editor_template	= editor_template;
		}

		public Type			edited_type { get; set; }

		public override bool can_edit(property_grid_property property)
		{
			if (property.descriptors[0].PropertyType == edited_type)
				return true;
			return false;
		}
	}
}
