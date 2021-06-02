////////////////////////////////////////////////////////////////////////////
//	Created		: 28.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls.curve_editor
{
	internal static class hermite_spline_evaluator
	{
		private static readonly	Single[]		m_f_coeff = new Single[4];
	
		private	static		void			hermite_create			( Single[] x, Single[] y )				
		{
			float dx, dy, tan_x, m1, m2, length, d1, d2;

			/*	
			 *	Compute the difference between the 2 keyframes.					
			 */
			dx = x[3] - x[0];
			dy = y[3] - y[0];

			/* 
			 * 	Compute the tangent at the start of the curve segment.			
			 */
			tan_x = x[1] - x[0];
			m1 = m2 = 0.0f;
			if (tan_x != 0.0) {
				m1 = (y[1] - y[0]) / tan_x;
			}

			tan_x = x[3] - x[2];
			if (tan_x != 0.0) {
				m2 = (y[3] - y[2]) / tan_x;
			}

			length = 1.0f / (dx * dx);
			d1 = dx * m1;
			d2 = dx * m2;
			m_f_coeff[0] = (d1 + d2 - dy - dy) * length / dx;
			m_f_coeff[1] = (dy + dy + dy - d1 - d1 - d2) * length;
			m_f_coeff[2] = m1;
			m_f_coeff[3] = y[0];
		}
		private static		Double			de_evaluate				( Double a, Double b, Double c, Double d, Double res, ref Double[] ret )
		{
			var ret_val = Double.MaxValue;

			if( a != 0 )
			{
				b	/= a;
				c	/= a;
				d	/= a;
				d	-= res / a;

				var num = Cubic( ref ret, b, c, d );

				ret_val = num == 3 ? ret[2] : ret[0];
			}
			else
			{
				if( b!= 0 )
				{
					a = b;
					b = c;
					c = d;

					c -= res;
					var D = b*b - 4*a*c;

					if( D > 0 )
					{
						ret_val = ( -b + Math.Sqrt( D ) ) / ( 2 * a );
						var x2 = ( -b - Math.Sqrt( D ) ) / ( 2 * a );
					}
					else
					{
						ret_val = -b/(2*a);
					}
				}
				else
				{
					if( c != 0 )
					{
						ret_val = ( ( res - d ) / c );
					}
					else
					{
						//error
					}

				}
			}

			return ret_val;
		}

		private static		Double	sgn		( Double x )	
		{
			return x > 0 ? 1 : -1;
		}
		private static		Double	ch		( Double x )	
		{
			return ( Math.Pow( Math.E, x ) + Math.Pow( Math.E, -x ) ) / 2;
		}
		private static		Double	sh		( Double x )	
		{
			return ( Math.Pow( Math.E, x ) - Math.Pow( Math.E, -x ) ) / 2;
		}
		private static		Double	Arch	( Double x )	
		{
			var pow = Math.Sqrt( x*x - 1 );
			return Math.Log( x + pow, Math.E );
		}
		private static		Int32	Cubic	( ref Double[] x, Double a, Double b, Double c )	
		{
			var M_2PI = Math.PI*2;

			double q,r,r2,q3;
			q = ( a*a-3.0*b)/9.0;
			r = ( a*(2.0*a*a-9.0*b)+27.0*c)/54.0;
			
			r2=r*r;
			q3=q*q*q;
			
			if(  r2 < q3 )
			{
				var t = Math.Acos(r/ Math.Sqrt(q3));
				a/=3.0;
				q=-2.0*Math.Sqrt(q);
				x[0]=q*Math.Cos(t/3.0)-a;
				x[1]=q*Math.Cos((t+M_2PI)/3.0)-a;
				x[2]=q*Math.Cos((t-M_2PI)/3.0)-a;
				return( 3 );
			}
			else
			{
				//var fi = Arch( Math.Abs(r) / Math.Pow( Math.Abs(q), 3.0/2.0 ) ) / 3;
				//if( Double.IsNaN( fi ) )
				//{
				//}
				//x[0] = -2 * sgn( r )*Math.Pow( Math.Abs( q ), 1.0/2.0 )*ch(fi)-a/3;

				//x[1] = sgn( r )*Math.Pow( Math.Abs( q ), 1.0/2.0 )*ch(fi) - a/3 + Math.Pow( 3 * Math.Abs( q ) , 1.0/2.0 ) * sh(fi);
				//x[2] = sgn( r )*Math.Pow( Math.Abs( q ), 1.0/2.0 )*ch(fi) - a/3 - Math.Pow( 3 * Math.Abs( q ) , 1.0/2.0 ) * sh(fi);

				//return 1;
				double aa;
				double bb;

				if(r<=0.0)
				    r=-r;

				aa=-Math.Pow(r+Math.Sqrt(r2-q3),1.0/3.0); 

				if(aa!=0.0)
				    bb=q/aa;
				else
				    bb=0.0;

				a /=	3.0;
				q =		aa+bb;
				r =		aa-bb; 

				x[0]=q-a;
				x[1]=(-0.5)*q-a;
				x[2]=(Math.Sqrt(3.0)*0.5)*Math.Abs(r);

				return x[2]==0.0 ? 2 : 1;
			}
		}

		public static		void			hermite_create			( float_curve_key left_key, float_curve_key right_key )					
		{
			var x		= new Single[4];  
			var y		= new Single[4];  

			x[0]		= (Single)left_key.position.X;
			y[0]		= (Single)left_key.position.Y;
			x[1]		= (Single)left_key.right_tangent.X;
			y[1]		= (Single)left_key.right_tangent.Y;

			x[2] 		= (Single)right_key.left_tangent.X;
			y[2] 		= (Single)right_key.left_tangent.Y;
			x[3] 		= (Single)right_key.position.X;
			y[3] 		= (Single)right_key.position.Y;

			hermite_create( x, y );
		}
		public static 		Single			evaluate				( Single t )															
		{
			return (t * (t * (t * m_f_coeff[0] + m_f_coeff[1]) + m_f_coeff[2]) + m_f_coeff[3]);
		}
		public static		Double			de_evaluate				( float_curve_key left_key, float_curve_key right_key, Point position )	
		{
			var x_res = new Double[3];
			//var y_res = new Double[3];
			
			var new_t = de_evaluate( m_f_coeff[0], m_f_coeff[1], m_f_coeff[2], m_f_coeff[3], position.Y, ref x_res );
			var pos_y = evaluate( (Single)new_t );

			if( Math.Abs( pos_y - position.Y ) > 0.01 )
			{

			}

		//    var res		= Double.MaxValue;
		//    for( var xi = 0; xi < x_res.Length; ++xi )
		//    {
		//        if( x_res[xi] > 1.001 || x_res[xi] < -0.001 )
		//        {
		//            x_res[xi] = -1;
		//            continue;
		//        }
		//        if( res == Double.MaxValue )
		//            res = x_res[xi];

		//        for( var yi = 0; yi < y_res.Length; ++yi )	
		//        {
		//            if( y_res[yi] > 1.001 || y_res[yi] < -0.001 )
		//            {
		//                y_res[yi] = -1;
		//                continue;
		//            }
		//            if( Math.Abs( x_res[xi] - y_res[yi] ) < 0.001 )
		//            {
		//                res = x_res[xi];
		//                goto for_end;
		//            }
		//        }
		//    }
		//for_end:
		//    //if( Math.Abs( time - res ) > 0.001 )
		//    //{

		//    //}

		//    //var ret_val = (t * (t * (t * m_f_coeff[0] + m_f_coeff[1]) + m_f_coeff[2]) + m_f_coeff[3]);

		//    //if( Math.Abs( t - new_t ) > 0.0001 )
		//    //{
		//    //}

			return new_t;
		}

	}
}
