using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Windows;

namespace xray.editor.wpf_controls
{
	class bool_to_visibility_converter: IValueConverter
	{
		public Boolean need_to_hide { get; set; }		
		public Boolean need_to_reverse { get; set; }		
		
		#region IValueConverter Members

		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			Boolean val = (Boolean)value;
			return ((need_to_reverse)?(!val):val)?Visibility.Visible:((need_to_hide)?Visibility.Hidden:Visibility.Collapsed);
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}

		#endregion
	}
}
