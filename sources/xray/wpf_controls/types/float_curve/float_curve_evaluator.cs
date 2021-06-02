////////////////////////////////////////////////////////////////////////////
//	Created		: 21.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class float_curve_evaluator: hermite_spline_evaluator
	{
		public float_curve_evaluator( float_curve curve )
		{
			m_curve			= curve;
			m_left_key		= m_curve.keys[0];
			m_right_key		= m_curve.keys.Count > 1 ? m_curve.keys[1] : m_curve.keys[0];

			hermite_create	( m_left_key, m_right_key );
		}

		private readonly	float_curve			m_curve;
		private				float_curve_key		m_left_key;
		private				float_curve_key		m_right_key;
		private				Int32				m_current_segment_index;

		public				void				hermite_create			( float_curve_key left_key, float_curve_key right_key )		
		{
			create_hermite( left_key.position, left_key.right_tangent, right_key.left_tangent, right_key.position );
		}

		public				Point				evaluate				( Double x_position )										
		{
			if( m_curve.keys.Count == 1 )
				return m_curve.keys[0].position;

			while( x_position > m_right_key.position_x )
			{
				++m_current_segment_index;
				if( m_curve.keys.Count <= 2 )
					m_current_segment_index = 0;

				if( m_current_segment_index > m_curve.keys.Count - 2 )
					m_current_segment_index = m_curve.keys.Count - 2;

				m_left_key	= m_curve.keys[m_current_segment_index];
				m_right_key = m_curve.keys[m_current_segment_index + 1];
			}

			while( x_position < m_left_key.position_x )
			{
				--m_current_segment_index;
				if( m_curve.keys.Count <= 2 )
					m_current_segment_index = 0;

				if( m_current_segment_index < 0 )
					m_current_segment_index = 0;

				m_left_key	= m_curve.keys[m_current_segment_index];
				m_right_key	= m_curve.keys[m_current_segment_index + 1];
			}

			hermite_create	( m_left_key, m_right_key );

			var on_spline_position = x_position - m_left_key.position_x;
			var y = evaluate_spline( on_spline_position );

			return new Point( x_position, y );
		}
	}
}
