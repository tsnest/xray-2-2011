using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace xray.editor.wpf_controls
{
	class category_group_description : GroupDescription
	{
		public override object GroupNameFromItem(object item, int level, System.Globalization.CultureInfo culture)
		{
			var attribute = (CategoryAttribute)((property_grid_property)item).descriptors[0].Attributes[typeof(CategoryAttribute)];
			if (attribute != null)
				return attribute.Category.Trim();

			return "Misc";
		}
	}
}
