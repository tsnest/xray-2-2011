////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Media;

namespace xray.editor.wpf_controls
{
	/// <summary>
	///  Describes a color in terms of Hue, Saturation, and Value (brightness)
	/// </summary>
	public struct color_hsv
	{

		public			color_hsv		( Double h, Double s, Double v )				
		{
			this.h = h;
			this.s = s;
			this.v = v;
			a = 1;
		}
		public			color_hsv		( Double h, Double s, Double v, Double a )		
		{
			this.h = h;
			this.s = s;
			this.v = v;
			this.a = a;
		}

		public			Double			h;
		public			Double			s;
		public			Double			v;
		public			Double			a;

		public static	explicit operator		color_hsv	( color_rgb color )						
		{
			return color_utilities.convert_rgb_to_hsv( color );
		}
		public static	explicit operator		color_hsv	( Color color )							
		{
			return color_utilities.convert_rgb_to_hsv( color );
		}
		public static	explicit operator		Color		( color_hsv color )						
		{
			return (Color)color_utilities.convert_hsv_to_rgb( color );
		}
		public static	Boolean					operator ==	( color_hsv first, color_hsv second )	
		{
			return first.h == second.h && first.s == second.s && first.v ==  second.v && first.a == second.a;
		}
		public static	Boolean					operator !=	( color_hsv first, color_hsv second )	
		{
			return !( first == second );
		}

		public override Boolean					Equals		( Object obj )							
		{
			return obj is color_hsv && (color_hsv)obj == this;
		}
		public override Int32					GetHashCode	( )										
		{
			return (Int32)( ( (Double)h.GetHashCode( ) ) / s.GetHashCode( ) * v.GetHashCode( ) / a.GetHashCode( ) ); 
				
		}

	}
}
