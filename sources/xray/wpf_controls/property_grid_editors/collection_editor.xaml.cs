using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections;

namespace xray.editor.wpf_controls.property_grid_editors
{
	/// <summary>
	/// Interaction logic for collection_editor.xaml
	/// </summary>
	public partial class collection_editor : UserControl
	{
		public collection_editor()
		{
			InitializeComponent();
		}

		private void add_Click(object sender, RoutedEventArgs e)
		{
			property_grid_property collection_property = ((property_grid_property)((FrameworkElement)sender).DataContext);

			property_grid_property property = new property_grid_property(collection_property.owner_property_grid);
			property.name = "item";//+((IList)collection_property.value).Count;
			property.is_collection_item = true;
			property.property_parent = collection_property;

			foreach (IList list in collection_property.values)
			{
				Type[] generic_types  = list.GetType().GetGenericArguments();
				if(generic_types != null && generic_types.Length > 0)
					list.Add(Activator.CreateInstance(generic_types[0]));
				else
					list.Add(new Object());
				
				property.descriptors.Add(new property_grid_item_property_descriptor(property.name, list.Count-1, list));
				property.property_owners.Add(collection_property.value);
			}

			collection_property.sub_properties.Add(property);
		}
	}

	internal class collection_editor_is_fixed_size_checker : IValueConverter
	{

		#region IValueConverter Members

		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			return (((IList)value).IsFixedSize)?Visibility.Hidden:Visibility.Visible;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}

		#endregion
	}
}
