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
	class singles_to_thickness_converter: IMultiValueConverter
	{
		public singles_to_thickness_converter()
		{
			sides = new []{0, 1, 2, 3};
		}

		public Int32[] sides
		{
			get;set;
		}

		private static void set_thickness_prop( ref Thickness thickness, Int32 side, Object value )
		{
			if( value == null )
				value = (Single)0;

			if( !( value is Single ) )
			{
				thickness.Left = 0;
				return;
			}

			var val = (Single)System.Convert.ChangeType( value, typeof(Single) );
			switch( side )
			{
				case 0: thickness.Left		= val; break;
				case 1: thickness.Top		= val; break;
				case 2: thickness.Right		= val; break;
				case 3: thickness.Bottom	= val; break;
				default: throw new ArgumentException("Side must be inside 0-3 range");
			}
		}

		public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
		{
			if( values == null || values.Length == 0 )
				return null;

			var thickness = new Thickness();
			var count = values.Length;
			for ( var i = 0; i < count; ++i )
				set_thickness_prop( ref thickness, sides[i], values[i] );
			return thickness;
		}

		public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}
