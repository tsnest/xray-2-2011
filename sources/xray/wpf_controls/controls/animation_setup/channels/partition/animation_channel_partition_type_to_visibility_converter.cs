////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows;

namespace xray.editor.wpf_controls.animation_setup
{
	internal class animation_channel_partition_type_to_visibility_converter: IValueConverter
	{
		public Object	Convert				( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			return ( value.ToString() == parameter.ToString() ) ? Visibility.Visible : Visibility.Collapsed;
		}
		public Object	ConvertBack			( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			throw new NotImplementedException();
		}
	}
}
