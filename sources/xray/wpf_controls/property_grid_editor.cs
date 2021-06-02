using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class property_grid_editor
	{
		public virtual	Boolean			can_edit(property_grid_property property)
		{
			return false;
		}
		public			DataTemplate	editor_template { get; set; }
	}
}
