////////////////////////////////////////////////////////////////////////////
//	Created		: 18.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace xray.editor.wpf_controls.converters
{
	public class color_to_brush_converter: IValueConverter
	{
		public Object Convert( Object value, Type target_type, Object parameter, CultureInfo culture )
		{
			if( value is Color )
				return new SolidColorBrush( (Color)value );

			if( value is color_rgb )
				return new SolidColorBrush( (Color)(color_rgb)value );

			if( value is color_hsv )
				return new SolidColorBrush( (Color)(color_hsv)value );

			return  new SolidColorBrush(Colors.Black);
		}

		public Object ConvertBack( Object value, Type target_type, Object parameter, CultureInfo culture)
		{
			return System.Convert.ChangeType( ( (SolidColorBrush)value ).Color, target_type );
		}
	}
}
