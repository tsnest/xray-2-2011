////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace xray.editor.wpf_controls.converters
{
	class single_to_thickness_converter: IValueConverter
	{
		public	Int32	side
		{
			get;set;
		}

		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var thickness = new Thickness();
			switch( side )
			{
				case 0: thickness.Left		= (Single)value; break;
				case 1: thickness.Top		= (Single)value; break;
				case 2: thickness.Right		= (Single)value; break;
				case 3: thickness.Bottom	= (Single)value; break;
				default: throw new ArgumentException("Side must be inside 0-3 range");
			}
			return thickness;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
