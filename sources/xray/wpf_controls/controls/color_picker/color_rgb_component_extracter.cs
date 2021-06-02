////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls.color_picker
{
	internal class color_rgb_component_extracter: IValueConverter
	{
		public object Convert( Object value, Type target_type, Object parameter, CultureInfo culture )
		{
			var color = color_utilities.convert_hsv_to_rgb( (color_hsv)value );

			if( parameter != null )
			{
				switch( Int32.Parse( parameter.ToString( ) ) )
				{
					case 0:
						return color.r;
					case 1:
						return color.g;
					case 2:
						return color.b;
					case 3:
						return color.a;
				}
			}

			return color.r;
		}
		public object ConvertBack( object value, Type target_type, object parameter, CultureInfo culture )
		{
			throw new NotImplementedException( );
		}
	}
}
