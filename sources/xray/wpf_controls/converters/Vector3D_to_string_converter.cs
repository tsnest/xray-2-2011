////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media.Media3D;

namespace xray.editor.wpf_controls.converters
{
	public class Vector3D_to_string_converter: IValueConverter
	{
		public	Vector3D_to_string_converter	( Action<Vector3D> converted_callback )		
		{
			m_converted_callback = converted_callback;
		}

		readonly Action<Vector3D> m_converted_callback;

		public	Object			Convert		( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			if ( value != null )
			{
				var vector = (Vector3D)value;
				return String.Format("{0:N3} {1:N3} {2:N3}", vector.X, vector.Y, vector.Z );
			}
			
			return "<many>";
		}
		public	Object			ConvertBack	( Object value, Type target_type, Object parameter, CultureInfo culture )	
		{
			var str			= (String)value;
			var ret_value	= new Vector3D( );
			var components	= str.Split( str.IndexOf(';') != -1 ? ';' : ' ' );

			Double out_rez;
			Boolean is_parsed;

			if( components.Length > 0 )
			{
				is_parsed = Double.TryParse( components[0], out out_rez );
				ret_value.X = is_parsed ? out_rez : 0;
			}
			if( components.Length > 1 )
			{
				is_parsed = Double.TryParse( components[1], out out_rez );
				ret_value.Y = is_parsed ? out_rez : 0;
			}

			if( components.Length > 2 )
			{
				is_parsed = Double.TryParse( components[2], out out_rez );
				ret_value.Z = is_parsed ? out_rez : 0;
			}
			
			if(m_converted_callback != null)
				m_converted_callback( ret_value );

			return ret_value;
		}
	}
}
