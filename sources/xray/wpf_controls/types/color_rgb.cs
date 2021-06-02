////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Describes HDR RGB Color
	/// </summary>
	public struct color_rgb: i_caster
	{
		public			color_rgb		( Double r, Double g, Double b )				
		{
			this.r = r;
			this.g = g;
			this.b = b;
			a = 1;
		}
		public			color_rgb		( Double r, Double g, Double b, Double a  )		
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = a;
		}

		public			Double			r;
		public			Double			g;
		public			Double			b;
		public			Double			a;

		public			Double			multily_factor		
		{
			get
			{
				return Math.Max( 1, Math.Max( r, Math.Max( g, b ) ) );
			}
		}

		public static	explicit operator		color_rgb	( Color color )							
		{
			return new color_rgb( color.ScR, color.ScG, color.ScB, color.ScA );
		}
		public static	explicit operator		Color		( color_rgb color )						
		{
			color.multiply( 1 / color.multily_factor );
			return new Color{ ScR = (Single)color.r, ScG = (Single)color.g, ScB = (Single)color.b, ScA = (Single)color.a };
		}
		public static	explicit operator		color_rgb	( color_hsv color )						
		{
			return color_utilities.convert_hsv_to_rgb( color );
		}
		public static	explicit operator		color_rgb	( Point4D color )						
		{
			return new color_rgb( color.X, color.Y, color.Z, color.W );
		}
		public static	explicit operator		Point4D		( color_rgb color )						
		{
			return new Point4D{ X = (Single)color.r, Y = (Single)color.g, Z = (Single)color.b, W = (Single)color.a };
		}
		public static	Boolean					operator ==	( color_rgb first, color_rgb second )	
		{
			return first.a == second.a && first.r == second.r && first.g == second.g && first.b == second.b;
		}
		public static	Boolean					operator !=	( color_rgb first, color_rgb second )	
		{
			return ! ( first == second );
		}
		private	static	Double					learp		( Double first, Double second, Double offset )			
		{
			return first + offset * ( second - first );
		}

		public override Boolean					Equals		( Object obj )							
		{
			return obj is color_rgb && (color_rgb)obj == this;
		}
		public override Int32					GetHashCode	( )										
		{
			return (Int32)( ( (Double)r.GetHashCode( ) ) / g.GetHashCode( ) * b.GetHashCode( ) / a.GetHashCode( ) ); 
				
		}
		public			Object					cast_to		( Type type )							
		{
			if( type == typeof( Color ) )
			{
				return (Color)this;
			}
			return this;
		}
		public			void					multiply	( Double value )						
		{
			r *= value;
			g *= value;
			b *= value;
		}

		public static	color_rgb				learp		( color_rgb first, color_rgb second, Double offset )	
		{
			return new color_rgb(
				learp( first.r, second.r, offset ),
				learp( first.g, second.g, offset ),
				learp( first.b, second.b, offset ),
				learp( first.a, second.a, offset )
			);
		}

		public override String					ToString	( )										
		{
			return "a:" + a + " r:" + r + " g:" + g + " b:" + b;
		}

	}
}
