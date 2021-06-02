////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Evgheniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;

namespace xray.editor.wpf_controls.converters
{
	class Double_to_String_converter: IValueConverter
	{
		public Double_to_String_converter( )
		{
			
		}
	
		public	String	prefix
		{
			get;set;
		}
	
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			Double val = 0;
			if ( value != null )
				val = (Double)value;
			
			return String.Format( prefix+"{0:N3}", val );
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			var str				= (String)value;
			Double ret_value;

			Double	out_rez;
			Boolean is_parsed;

			var chars = new char[ prefix.Length+1 ];
			chars[0] = ' ';
			for( int i = 0; i < prefix.Length; ++i )
				chars[i+1] = prefix[i];
			var tmp = str.Trim( chars );
			is_parsed = Double.TryParse( tmp, out out_rez );
			ret_value = is_parsed ? out_rez : 0;

			return ret_value;
		}
	}
}
