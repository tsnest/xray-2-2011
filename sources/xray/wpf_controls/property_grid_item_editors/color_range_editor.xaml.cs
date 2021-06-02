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

namespace xray.editor.wpf_controls.property_grid_item_editors
{
	/// <summary>
	/// Interaction logic for color_range_editor.xaml
	/// </summary>
	public partial class color_range_editor : UserControl
	{
		public color_range_editor()
		{
			InitializeComponent();
		}

		private void expand_collapse_Click(object sender, RoutedEventArgs e)
		{
			var border = body;
			var property_item = (property_grid_property)this.DataContext;

			if (border.Visibility == Visibility.Collapsed)
			{
				property_item.is_expanded = true;
				expand.Visibility = Visibility.Collapsed;
				collapse.Visibility = Visibility.Visible;
				border.Visibility = Visibility.Visible;
			}
			else
			{
				property_item.is_expanded = false;
				expand.Visibility = Visibility.Visible;
				collapse.Visibility = Visibility.Collapsed;
				border.Visibility = Visibility.Collapsed;
			}
		}
	}
}
