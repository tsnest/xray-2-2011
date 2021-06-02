using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls
{
	public class time_layout_parent_length_time_converter : IValueConverter
	{
		private time_layout m_parent;

		public time_layout_parent_length_time_converter(time_layout parent)
		{
			m_parent = parent;
		}

		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var ret_value = (float)value;

			return ret_value.ToString("F1");
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var new_value = value.ToString();
			float result;
			if (float.TryParse(new_value, NumberStyles.Any, culture,out result)){
				return result;
			}
			throw new NotImplementedException();
		}
	}
}
