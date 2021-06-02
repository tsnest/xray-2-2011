using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls
{
	public class time_layout_end_time_converter : IMultiValueConverter
	{
		public time_layout_end_time_converter(time_layout_item_control parent)
		{
			m_parent = parent;
		}

		private float m_start_time;
		private time_layout_item_control m_parent;

		public object Convert(object[] values, Type target_type, object parameter,
								CultureInfo culture)
		{
			if(values[0] is float && values[1] is float)
			{
				m_start_time = (float) values[0];
				var length_time_value = (float) values[1];
				return (m_start_time + length_time_value).ToString("F1");
			}
			return "--";
		}

		public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
		{
			object[] values = new object[2];
			values[0] = m_start_time;
			values[1] = float.Parse((String)value) - m_start_time;
			return values;
		}
	}
}
