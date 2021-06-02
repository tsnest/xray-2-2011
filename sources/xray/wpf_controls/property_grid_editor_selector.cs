using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.ComponentModel;

namespace xray.editor.wpf_controls
{
	class property_grid_editor_selector: DataTemplateSelector
	{
		public property_grid_editor_selector(property_grid propertyGrid)
		{
			_propertyGrid = propertyGrid;
		}

		private property_grid _propertyGrid;
		private ReadOnlyAttribute m_read_only_attribute = new ReadOnlyAttribute(true);

		public override DataTemplate SelectTemplate(object item, DependencyObject container)
		{
			if ((item as property_grid_property).descriptors[0].Attributes.Matches(m_read_only_attribute))
				return (DataTemplate)((FrameworkElement)container).FindResource("default_editor");

			if ((item as property_grid_property).is_multiple_values)
				return (DataTemplate)((FrameworkElement)container).FindResource("many_editor");

			foreach (property_grid_editor propEditor in _propertyGrid.editors)
			{
				if (propEditor.can_edit((property_grid_property)item))
					return propEditor.editor_template;
			}

			if( TypeDescriptor.GetProperties(((property_grid_property)item).value).Count > 0)
				((property_grid_property)item).is_expandable_item = true;
			return (DataTemplate)((FrameworkElement)container).FindResource("default_editor");
		}
	}
}
