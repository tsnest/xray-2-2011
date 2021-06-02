using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.ComponentModel;
using System.Windows;

namespace xray.editor.wpf_controls
{
	class property_grid_item_editor_selector: DataTemplateSelector
	{
		public property_grid_item_editor_selector(property_grid propertyGrid)
		{
			_propertyGrid = propertyGrid;
		}

		private property_grid _propertyGrid;
		private ReadOnlyAttribute m_read_only_attribute = new ReadOnlyAttribute(true);

		public override DataTemplate SelectTemplate(object item, DependencyObject container)
		{
			foreach (property_grid_editor item_editor in _propertyGrid.item_editors)
			{
				if (item_editor.can_edit((property_grid_property)item))
					return item_editor.editor_template;
			}

			return (DataTemplate)((FrameworkElement)container).FindResource("common_item_editor");
		}
	}
}
