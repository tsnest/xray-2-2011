using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Data;
using System.ComponentModel;
using System.Windows.Controls;
using Flobbster.Windows.Forms;
using System.Collections;

namespace xray.editor.wpf_controls.property_grid_editors
{
	partial class combo_box_editor : UserControl
	{
		public combo_box_editor()
		{
			InitializeComponent();

			DataContextChanged += (o, i) =>
			{
				items_attribute = (combo_box_items_attribute)((property_grid_property)DataContext).descriptors[0].Attributes[typeof(combo_box_items_attribute)];
			};
		}

		combo_box_items_attribute items_attribute;

		private void UserControl_Loaded(object sender, RoutedEventArgs e)
		{
			if (items_attribute.items_count_func != null)
				set_source_from_attribute();
			else
				combo_box.ItemsSource = items_attribute.items;
		}

		void set_source_from_attribute()
		{
			Int32 count = items_attribute.items_count_func();
			ArrayList list = new ArrayList();
			for (int i = 0; i < count; ++i)
				list.Add(items_attribute.get_item_func(i));
			combo_box.ItemsSource = list;
		}

		private void combo_box_PreviewMouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			if (items_attribute.items_count_func != null)
				set_source_from_attribute();
		}
	}

	public class combo_box_items_attribute : Attribute
	{
		public combo_box_items_attribute(ArrayList items)
		{
			this.items = items;
		}

		public combo_box_items_attribute(Func<Int32> items_count_func, Func<Int32, String> get_item_func)
		{
			this.items_count_func = items_count_func;
			this.get_item_func = get_item_func;
		}

		public ArrayList items;
		public Func<Int32> items_count_func;
		public Func<Int32, String> get_item_func;
	}
}
