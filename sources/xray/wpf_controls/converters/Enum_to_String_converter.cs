using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls.converters
{
	class Enum_to_String_converter: IValueConverter
	{
		Type last_converted_type;

		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
            if (value == null)
                return null;

			last_converted_type = value.GetType();
			return value.ToString();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			return Enum.Parse(last_converted_type, (String)value);
		}
	}
}
