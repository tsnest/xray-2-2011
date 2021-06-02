////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace xray.editor.wpf_controls.converters
{
	internal class color_to_string_converter: IValueConverter
	{
		public		Object		Convert			( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			var color = ( value is color_rgb ) ? (color_rgb)value : (color_rgb)(Color)value;
			return color.ToString( );
		}
		public		Object		ConvertBack		( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			throw new NotImplementedException( );
		}
	}
}
