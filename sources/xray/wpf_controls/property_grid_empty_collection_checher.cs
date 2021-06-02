using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Windows;

namespace xray.editor.wpf_controls
{
	class property_grid_empty_collection_checher: IValueConverter
	{
		#region IValueConverter Members

		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			var properties = ((property_grid_property)value).sub_properties;

			if(properties != null)
				return (properties.Count != 0)?Visibility.Visible:Visibility.Collapsed;
			else
				return Visibility.Visible;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}

		#endregion
	}
}
