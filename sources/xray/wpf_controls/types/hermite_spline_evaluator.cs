////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class hermite_spline_evaluator
	{
		private				Double				m_coeff_0;
		private				Double				m_coeff_1;
		private				Double				m_coeff_2;
		private				Double				m_coeff_3;

		public				void				create_hermite			( Point pt0, Point pt1, Point pt2, Point pt3 )		
		{
			var m1 = 0.0;
			var m2 = 0.0;

			//Compute the difference between the 2 keyframes.
			var dx = pt3.X - pt0.X;
			var dy = pt3.Y - pt0.Y;

			//Compute the tangent at the start of the curve segment.
			var tan_dx = pt1.X - pt0.X;
			if ( tan_dx != 0.0 ) {
				m1 = ( pt1.Y - pt0.Y ) / tan_dx;
			}
			//Compute the tangent at the end of the curve segment.
			tan_dx = pt3.X - pt2.X;
			if ( tan_dx != 0.0 ) {
				m2 = ( pt3.Y - pt2.Y ) / tan_dx;
			}

			//Compute hermite coefficients.
			var length = 1.0f / ( dx * dx );
			var d1 = dx * m1;
			var d2 = dx * m2;
			m_coeff_0 = (d1 + d2 - dy - dy) * length / dx;
			m_coeff_1 = (dy + dy + dy - d1 - d1 - d2) * length;
			m_coeff_2 = m1;
			m_coeff_3 = pt0.Y;
		}
		public				Double				evaluate_spline			( Double t )												
		{
			return ( t * ( t * ( t * m_coeff_0 + m_coeff_1 ) + m_coeff_2 ) + m_coeff_3 );
		}
	}
}
