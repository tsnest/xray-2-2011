using System;
using System.Data;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls
{
	public class time_layout_length_time_converter : IValueConverter
	{
		private readonly time_layout_item_control m_parent;

		public time_layout_length_time_converter(time_layout_item_control parent)
		{
			m_parent = parent;
		}

		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if ((float)value < 0){
				
			}
			return ((float)value).ToString("F1");
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var new_value = value.ToString();
			float result;
			if (float.TryParse(new_value, NumberStyles.Any, culture,out result)){
				((time_layout_item)(m_parent.DataContext)).start_time += (((time_layout_item)(m_parent.DataContext)).length_time - result)/2;
				return result;
			}
			throw new NotImplementedException();
		}
	}
}
