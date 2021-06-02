////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls.color_picker
{
	internal class color_to_hue_part_converter: IValueConverter
	{
		public object Convert( object value, Type target_type, object parameter, CultureInfo culture )
		{
			return color_utilities.convert_hsv_to_rgb( color_utilities.convert_rgb_to_hsv( (color_rgb)value ).h, 1, 1, 1 );
		}
		public object ConvertBack( object value, Type target_type, object parameter, CultureInfo culture )
		{
			throw new NotImplementedException( );
		}
	}
}
