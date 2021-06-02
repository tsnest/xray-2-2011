////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls.converters
{
	internal class number_to_double_converter: IValueConverter
	{
		public Object Convert( Object value, Type target_type, Object parameter, CultureInfo culture )
		{
			return System.Convert.ToDouble( value );
		}

		public Object ConvertBack( Object value, Type target_type, Object parameter, CultureInfo culture )
		{
			return System.Convert.ChangeType( value, target_type );
		}
	}
}
