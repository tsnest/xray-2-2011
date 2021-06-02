////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Evgeny Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace xray.editor.wpf_controls.converters
{
	class Point_to_String_converter: IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var vector = new Point( 0,0 );
			if (value != null)
				vector = (Point)value;
			
			return String.Format("x{0:N3} y{1:N3}", vector.X, vector.Y );
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var str			= (String)value;
			var ret_value	= new Point();
			var components	= str.Split(str.IndexOf(';') != -1 ? ';' : ' ');

			Double out_rez;
			Boolean is_parsed;

			if( components.Length > 0 )
			{
				var tmp = components[0].Trim(' ', 'x');
				is_parsed = Double.TryParse( tmp, out out_rez );
				ret_value.X = is_parsed ? out_rez : 0;
			}
			if( components.Length > 1 )
			{
				var tmp = components[1].Trim(' ', 'y');
				is_parsed = Double.TryParse( tmp, out out_rez );
				ret_value.Y = is_parsed ? out_rez : 0;
			}

			return ret_value;
		}
	}
}
