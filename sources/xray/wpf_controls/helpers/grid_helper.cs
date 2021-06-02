////////////////////////////////////////////////////////////////////////////
//	Created		: 24.05.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public static class grid_helper
	{
		private static			Double			num_base				( Double number )									
		{
			var ret_number = 1.0;

			if( number >= 1 )
			{
				ret_number = Math.Pow( 10, (Int32)( Math.Log10( (Int32)number ) ) );
			}
			else
			{
				while( number < 1 )
				{
					number		*= 10;
					ret_number	*= 0.1;
				}
			}

			return ret_number;
		}
		
		public static			Double			scale_to_divide_factor	( out Double lines_per_segment, Double scale )		
		{
			Double divide_factor;
			var scale_base		= num_base( scale );

			scale /= scale_base;

			if( scale >= 5 )
				divide_factor = 0.2;

			else if( scale >= 2 )
				divide_factor = 0.5;

			else
				divide_factor = 1;
			
			lines_per_segment = (Int32)( 1 / divide_factor );
			divide_factor /= scale_base;

			return divide_factor * 10;
		}
		public static			void			compute_format_string	( Double divide_factor, out String format_string )	
		{
			if( divide_factor <= 1 )
			{
				var tmp_scale		= divide_factor;
				var after_don_num	= 0;
				while( tmp_scale < 1 )
				{
					tmp_scale *= 10;
					++after_don_num;
				}
				format_string = "{0:f" + ( after_don_num ) + "}";

				return;
			}

			if( divide_factor > 500 )
			{
				format_string = "{0:f0}k";
				return;
			}

			format_string = "{0:f0}";
		}
		public static			String			format					( String format_string, Double num )				
		{
			return format_string == "{0:f0}k" ? String.Format( format_string, num / 1000 ) : String.Format( format_string, num );
		}
	}
}
