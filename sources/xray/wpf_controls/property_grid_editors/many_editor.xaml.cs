using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Runtime.Serialization;

namespace xray.editor.wpf_controls.property_grid_editors
{
	partial class many_editor: ResourceDictionary
	{
		public many_editor()
		{
			InitializeComponent();
		}

		private void Button_Clicked(Object sender, RoutedEventArgs e)
		{
			property_grid_property prop = ((property_grid_property)((Button)sender).DataContext);
			if (prop.descriptors[0].PropertyType == typeof(String))
			    prop.value = "";
			else
			{
				try
				{
					prop.value = Activator.CreateInstance(prop.descriptors[0].PropertyType);
				}
				catch(MissingMethodException)
				{
					prop.value = FormatterServices.GetSafeUninitializedObject(prop.descriptors[0].PropertyType);
				}
			}

			prop.is_multiple_values = false;
			prop.owner_property_grid.reset();
		}
	}
}
