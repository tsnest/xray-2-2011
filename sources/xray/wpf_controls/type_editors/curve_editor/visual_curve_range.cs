////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Shapes;

namespace xray.editor.wpf_controls.curve_editor
{
	internal class visual_curve_range
	{
		public		visual_curve_range		( visual_curve curve )		
		{
			m_curve						= curve;
			m_curve.range				= this;
			
			m_curve.key_added			+= key_added;
			m_curve.key_removed			+= key_removed;
			m_curve.key_changed			+= process_key_changed;

			initialize_path				( );
			fill_work_segments			( );
			update_visual				( );
			update_start				( );
			update_end					( );
		}

		private				Path						m_path;
		private				PathFigure					m_path_figure;
		private readonly	visual_curve				m_curve;
		private				Int32						m_top_last_segment_index;

		private				LineSegment					m_top_first_segment;
		private				LineSegment					m_top_last_segment;
		private				LineSegment					m_bottom_first_segment;
		private				LineSegment					m_bottom_last_segment;
		private				LineSegment					m_top_to_bottom_segment;
		
		public				Path			path						
		{
			get
			{
				return m_path;
			}
		}

		private				void			initialize_path				( )									
		{
			var fill_color			= m_curve.float_curve.color;
			fill_color.ScA			= 0.1f;
			var stroke_color		= m_curve.float_curve.color;
			stroke_color.ScA		= 0.4f;

			m_path				= new Path{
				Fill	= new SolidColorBrush( fill_color ),
				Stroke	= new SolidColorBrush( stroke_color ),
				IsHitTestVisible = false,
				StrokeDashArray = {12, 5, 2, 5}
			};
			m_path.SetValue		( Panel.ZIndexProperty, 1 );

			m_path_figure					= new PathFigure( );
			m_top_first_segment				= new LineSegment( new Point( 0, 0 ), true );
			m_top_last_segment				= new LineSegment( new Point( 0, 0 ), true );
			m_top_to_bottom_segment			= new LineSegment( new Point( 0, 0 ), false );
			m_bottom_last_segment			= new LineSegment( new Point( 0, 0 ), true );
			m_bottom_first_segment			= new LineSegment( new Point( 0, 0 ), true );

			m_path_figure.Segments.Add		( m_top_first_segment );
			m_path_figure.Segments.Add		( m_top_last_segment );
			m_path_figure.Segments.Add		( m_top_to_bottom_segment );
			m_path_figure.Segments.Add		( m_bottom_last_segment );
			m_path_figure.Segments.Add		( m_bottom_first_segment );
			
			var path_geomerty				= new PathGeometry( );
			path_geomerty.Figures.Add		( m_path_figure );
			m_path.Data						= path_geomerty;
		}
		private				void			fill_work_segments			( )									
		{
			m_top_last_segment_index	= m_path_figure.Segments.IndexOf( m_top_last_segment );
			var work_segments_count		= m_top_last_segment_index - 1;

			var new_segments_count		= m_curve.keys.Count - 1 - work_segments_count;

			if( new_segments_count > 0 )
				for( var i = 0; i < new_segments_count; ++i )
				{
					m_path_figure.Segments.Insert	( 1, new BezierSegment{ IsStroked = true } );
					++m_top_last_segment_index;
					m_path_figure.Segments.Insert	( m_top_last_segment_index + 3, new BezierSegment{ IsStroked = true } );
				}
			if( new_segments_count < 0 )
				for( var i = 0; i < -new_segments_count; ++i )
				{
					m_path_figure.Segments.RemoveAt( 1 );
					--m_top_last_segment_index;
					m_path_figure.Segments.RemoveAt( m_top_last_segment_index + 3 );
				}
		}
		private				void			update_visual				( )									
		{
			var count = m_curve.keys.Count;
			for( var i = 0; i < count; ++i )
				process_key_changed			( i );
		}
		private				Point			up_to_delta					( Point point, Double delta )		
		{
			point.Y -= delta * m_curve.parent_panel.scale.Y;
			return point;
		}
		private				Point			down_to_delta				( Point point, Double delta )		
		{
			point.Y += delta * m_curve.parent_panel.scale.Y;
			return point;
		}
		private static		Double			keys_tangent_delta			( visual_curve_key first, visual_curve_key second )	
		{
			const Single c_one_thrid = 1.0f / 3;

			if( first.type_of_key == float_curve_key_type.linear )
				return	( second.key.range_delta - first.key.range_delta ) * c_one_thrid;

			return 0;
		}

		private				void			key_added					( visual_curve_key key )			
		{
			fill_work_segments	( );
			update_visual		( );
		}
		private				void			key_removed					( visual_curve_key key )			
		{
			fill_work_segments	( );
			update_visual		( );
		}
		private				void			process_key_changed			( Int32 key_index )					
		{
			update_left	( key_index );
			update_right( key_index );
		}
		public				void			update_left					( Int32 key_index )					
		{
			var bottom_segment_index		= m_top_last_segment_index + 3;
			var				key = m_curve.keys[key_index];
			BezierSegment	segment;

			if( key.is_first_key )
				m_top_first_segment.Point		= up_to_delta( m_curve.keys[0].visual_position, m_curve.keys[0].key.range_delta );
			else
			{
				var prev_key	= m_curve.keys[key_index - 1];

				segment			= ( (BezierSegment)m_path_figure.Segments[key_index] );
				segment.Point2	= up_to_delta( key.left_tangent_visual_point, key.key.range_delta + keys_tangent_delta( key, prev_key ) );
				segment.Point3	= up_to_delta( key.visual_position, key.key.range_delta );

				segment			= ( (BezierSegment)m_path_figure.Segments[bottom_segment_index + m_curve.keys.Count - key_index - 1] );
				segment.Point1	= down_to_delta( key.left_tangent_visual_point, key.key.range_delta + keys_tangent_delta( key, prev_key ) );
			}
		}
		public				void			update_right				( Int32 key_index )					
		{
			var bottom_segment_index		= m_top_last_segment_index + 3;
			var				key = m_curve.keys[key_index];
			BezierSegment	segment;

			if( !key.is_last_key )
			{
				var next_key	= m_curve.keys[key_index + 1];

				segment			= ( (BezierSegment)m_path_figure.Segments[key_index + 1] );
				segment.Point1	= up_to_delta( key.right_tangent_visual_point, key.key.range_delta + keys_tangent_delta( key, next_key ) );

				segment			= ( (BezierSegment)m_path_figure.Segments[bottom_segment_index + m_curve.keys.Count - key_index - 2] );
				segment.Point2	= down_to_delta( key.right_tangent_visual_point, key.key.range_delta + keys_tangent_delta( key, next_key ) );
				segment.Point3	= down_to_delta( key.visual_position, key.key.range_delta );
			}
			else
				m_bottom_last_segment.Point = down_to_delta( key.visual_position, key.key.range_delta );
		}
				
		internal			void			update_start				( )									
		{
			m_path_figure.StartPoint		= up_to_delta	( m_curve.visual_start_point, m_curve.keys[0].key.range_delta );
			m_bottom_first_segment.Point	= down_to_delta	( m_curve.visual_start_point, m_curve.keys[0].key.range_delta );
		}
		internal			void			update_end					( )									
		{
			m_top_last_segment.Point		= up_to_delta	( m_curve.visual_end_point, m_curve.keys[m_curve.keys.Count - 1].key.range_delta );
			m_top_to_bottom_segment.Point	= down_to_delta	( m_curve.visual_end_point, m_curve.keys[m_curve.keys.Count - 1].key.range_delta );
		}
	}
}