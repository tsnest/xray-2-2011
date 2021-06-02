////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Media;

namespace xray.editor.wpf_controls.curve_editor
{
	internal class visual_curve_segment
	{
		public		visual_curve_segment	( visual_curve curve, Int32 index, visual_curve_key left_key, visual_curve_key right_key )
		{
			m_curve		= curve;
			m_index		= index;
			m_left_key	= left_key;
			m_right_key	= right_key;

			m_bezier_segment = new BezierSegment(  );

			m_curve.segments.Insert				( index, this );
			m_curve.path_figure.Segments.Insert	( index + 1, m_bezier_segment );

			if( !is_last )
				m_curve.segments[index + 1].m_left_key		= m_right_key;

			if( !is_first )
				m_curve.segments[index - 1].m_right_key		= m_left_key;

			var count = m_curve.segments.Count;
			for( var i = m_index + 1; i < count; ++i )
				++m_curve.segments[i].m_index;
		}

		private				visual_curve			m_curve;
		private				Int32					m_index;
		private				visual_curve_key		m_left_key;
		private				visual_curve_key		m_right_key;
		private				BezierSegment			m_bezier_segment;

		internal			Int32					index						
		{
			get
			{
				return m_index;
			}
		}
		internal			Point					left_key_output_tangent		
		{
			get
			{
				return m_bezier_segment.Point1;
			}
		}
		internal			Point					right_key_input_tangent		
		{
			get
			{
				return m_bezier_segment.Point2;
			}
		}
		internal			Boolean					is_first					
		{
			get
			{
				return m_curve.segments[0] == this;
			}
		}
		internal			Boolean					is_last						
		{
			get
			{
				return m_curve.segments[m_curve.segments.Count - 1] == this;
			}
		}
		internal			visual_curve_segment	previous_segment			
		{
			get
			{
				return m_curve.segments[m_index - 1];
			}
		}
		internal			visual_curve_segment	next_segment				
		{
			get
			{
				return m_curve.segments[m_index + 1];
			}
		}

		internal			void					update			( )									
		{
			update_left		( );
			update_right	( );
		}
		internal			void					update_left		( )									
		{
			m_bezier_segment.Point1		= m_left_key.right_tangent_visual_point;
		}
		internal			void					update_right	( )									
		{
			m_bezier_segment.Point2		= m_right_key.left_tangent_visual_point;
			m_bezier_segment.Point3		= m_right_key.visual_position;
		}
		internal static		void					swap_keys		( visual_curve_segment segment )	
		{
			if( !segment.is_first )
				segment.previous_segment.m_right_key = segment.m_right_key;

			if( !segment.is_last )
				segment.next_segment.m_left_key = segment.m_left_key;

			utils.swap_values( ref segment.m_left_key, ref segment.m_right_key );
		}
		internal			void					remove			( )									
		{
			var count = m_curve.segments.Count;
			for( var i = m_index + 1; i < count; ++i )
				--m_curve.segments[i].m_index;

			if( !is_first )
				previous_segment.m_right_key = m_right_key;

			remove_visuals( );
		}
		internal			void					remove_visuals	( )									
		{
			m_curve.path_figure.Segments.Remove	( m_bezier_segment );
			m_curve.segments.RemoveAt			( m_index );
		}

		public override		String					ToString		( )									
		{
			return "index: " + m_index;
		}
	}
}
