using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls.converters
{
	public class main_dock_panel_width_converter : IMultiValueConverter
	{
		public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
		{
			var ret_value = (Double)values[0] + 500;
			if (ret_value < (Double)values[1])
				ret_value = (Double)values[1];
			return ret_value;
		}

		public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
