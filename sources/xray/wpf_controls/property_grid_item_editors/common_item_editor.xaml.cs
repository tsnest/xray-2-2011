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

namespace xray.editor.wpf_controls.property_grid_item_editors
{
	/// <summary>
	/// Interaction logic for common_item_editor.xaml
	/// </summary>
	public partial class common_item_editor : UserControl
	{
		public common_item_editor()
		{
			InitializeComponent();
		}

		private void expand_collapse_Click(object sender, RoutedEventArgs e)
		{
			var border = inner_data_border;
			var property_item = (property_grid_property)this.DataContext;

			if (sub_items.ItemsSource == null)
			{
				var prop_grid = ((property_grid_property)this.DataContext).owner_property_grid;
				property_item.sub_properties = prop_grid.get_properties_from_objects(property_item.values);

				foreach (var prop in property_item.sub_properties)
					prop.property_parent = property_item;

				sub_items.ItemTemplateSelector = prop_grid.m_item_editor_selector;
				sub_items.ItemsSource = property_item.sub_properties;
			}

			if (border.Visibility == Visibility.Collapsed)
			{
				property_item.is_expanded	= true;
				expand.Visibility			= Visibility.Collapsed;
				collapse.Visibility			= Visibility.Visible;
				border.Visibility			= Visibility.Visible;
			}
			else
			{
				property_item.is_expanded	= false;
				expand.Visibility			= Visibility.Visible;
				collapse.Visibility			= Visibility.Collapsed;
				border.Visibility			= Visibility.Collapsed;
			}
		}
		private void delete_item_Click(object sender, RoutedEventArgs e)
		{
			property_grid_property to_delete_property = ((property_grid_property)((FrameworkElement)sender).DataContext);

			int count = to_delete_property.property_owners.Count;
			for (int i = 0; i < count; ++i)
			{
				((IList)to_delete_property.property_owners[i]).RemoveAt(
					((property_grid_item_property_descriptor)to_delete_property.descriptors[i]).item_index
				);
			}

			Int32 index = to_delete_property.property_parent.sub_properties.IndexOf(to_delete_property);
			var properties = to_delete_property.property_parent.sub_properties;
			for (int i = index + 1; i < properties.Count; ++i)
			{
				foreach (property_grid_item_property_descriptor desc in properties[i].descriptors)
				{
					desc.decrease_item_index();
				}
			}

			to_delete_property.property_parent.sub_properties.RemoveAt(index);
		}
	}
}
