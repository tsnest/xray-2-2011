using System;
using System.Collections.Generic;
using System.Windows.Media;

namespace xray.editor.wpf_controls
{
	static class color_utilities
	{

		// Converts an RGB color to an HSV color.
		public static		color_hsv		convert_rgb_to_hsv			( Double r, Double g, Double b, Double a )			
		{
			Double h = 0;
			Double s;
			Double max;
			Double min;
			Double delta;

			max		= Math.Max(Math.Max(r, g), b);
			min		= Math.Min(Math.Min(r, g), b);
			delta	= max - min;

			if ( max == 0.0 )
				s = 0;
			else
				s = delta / max;

			if ( s == 0 )
				h = 0.0;
			else
			{
				if ( r == max )
					h = ( g - b ) / delta;
				else if ( g == max )
					h = 2 + ( b - r ) / delta;
				else if ( b == max )
					h = 4 + ( r - g ) / delta;

				h *= 60;
				if ( h < 0.0 )
					h = h + 360;
			}

			var v = max;

			return new color_hsv { h = h, s = s, v = v, a= a };
		}
		/// <summary>
		/// Converts an color_rgb to an color_hsb.
		/// </summary>
		/// <param name="color"></param>
		/// <returns></returns>
		public static		color_hsv		convert_rgb_to_hsv			( color_rgb color )						
		{
			return convert_rgb_to_hsv( color.r, color.g, color.b, color.a );
		}
		/// <summary>
		/// Converts an Media.Color color to an HSV color.
		/// </summary>
		/// <param name="color"></param>
		/// <returns></returns>
		public static		color_hsv		convert_rgb_to_hsv			( Color color )						
		{
			return convert_rgb_to_hsv( color.ScR, color.ScG, color.ScB, color.ScA );
		}
		// Converts an HSV color to an RGB color.
		public static		color_rgb			convert_hsv_to_rgb		( Double h, Double s, Double v, Double a )		
		{

			Double r;
			Double g;
			Double b;

			if (s == 0)
			{
				r = v;
				g = v;
				b = v;
			}
			else
			{
				Int32 i;
				Double f;
				Double p;
				Double q;
				Double t;

				if (h == 360)
					h = 0;
				else
					h = h / 60;

				i = (int)Math.Truncate(h);
				f = h - i;

				p = v * (1.0 - s);
				q = v * (1.0 - (s * f));
				t = v * (1.0 - (s * (1.0 - f)));

				switch (i)
				{
					case 0:
						r = v;
						g = t;
						b = p;
						break;

					case 1:
						r = q;
						g = v;
						b = p;
						break;

					case 2:
						r = p;
						g = v;
						b = t;
						break;

					case 3:
						r = p;
						g = q;
						b = v;
						break;

					case 4:
						r = t;
						g = p;
						b = v;
						break;

					default:
						r = v;
						g = p;
						b = q;
						break;
				}

			}

			return new color_rgb( r, g, b, a );
		}
		// Converts an HSV color to an RGB color.
		public static		color_rgb			convert_hsv_to_rgb			( color_hsv color )					
		{
			return convert_hsv_to_rgb( color.h, color.s, color.v, color.a );
		}
		// Generates a list of colors with hues ranging from 0 360 and a saturation and value of 1. 
		public static		List<Color>		generate_hsb_spectrum		( )									
		{

			var colors_list = new List<Color>( 30 );

			for ( var i = 0; i < 29; i++ )
				colors_list.Add( (Color)convert_hsv_to_rgb( i * 12, 1, 1, 1 ) );

			colors_list.Add( (Color)convert_hsv_to_rgb(0, 1, 1, 1) );

			return colors_list;
		}

	}
}
