////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows;

namespace xray.editor.wpf_controls.converters
{
	class bool_to_visibility_converter: IValueConverter
	{
		public Boolean	just_hide		
		{
			get; set; 
		}
		public Boolean	do_reverse		
		{
			get; set;
		}
		
		public Object	Convert				( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			var val = (Boolean)value;
			return ((do_reverse)?(!val):val)?Visibility.Visible:((just_hide)?Visibility.Hidden:Visibility.Collapsed);
		}
		public Object	ConvertBack			( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			throw new NotImplementedException();
		}
	}
}
