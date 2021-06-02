using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Windows.Data;

namespace xray.editor.wpf_controls
{
	class time_layout_scale_dependent_converter: IMultiValueConverter
	{
		private time_layout m_time_layout;

		public time_layout_scale_dependent_converter(time_layout parent)
		{
			m_time_layout = parent;
		}
		public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
		{
			return (Double)((float)values[0]*(float)values[1]);
		}

		public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
		{
			var values = new object[2];
            values[0] = (((Double)value)/m_time_layout.time_layout_scale);         
			values[1] = m_time_layout.time_layout_scale;
			return values;
		}
	}
}
