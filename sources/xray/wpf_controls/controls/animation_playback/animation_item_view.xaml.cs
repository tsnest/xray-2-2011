////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System;
using System.Windows.Media;
using System.Windows;

namespace xray.editor.wpf_controls.animation_playback
{
	/// <summary>
	/// Interaction logic for animation_item.xaml
	/// </summary>
	public partial class animation_item_view
	{

		#region | Initialize |


		public animation_item_view()
		{
			m_weights_min = 0.0f;
			m_weights_max = 1.0f;
			m_weights_last_pos = 0;

			m_timescale_min = 0.0f;
			m_timescale_max = 1.0f;
			m_timescale_last_pos = 0;
			InitializeComponent();

			DataContextChanged += delegate
			{
				m_item			= (animation_item)DataContext;
				m_item.view		= this;

				compute_curves_extremums	( );
				create_optimized_curves	( );
				fill_curves		( );
			};
		}


		#endregion

		#region |   Fields   |


		private				Single				m_weights_max;
		private				Single				m_weights_min;
		private				UInt32				m_weights_last_pos;

		private				Single				m_timescale_min;
		private				Single				m_timescale_max;
		private				UInt32				m_timescale_last_pos;

		private				animation_item		m_item;

		private readonly	List<Dictionary<UInt32, Single>>	m_scales			= new List<Dictionary<UInt32, Single>>( );
		private readonly	List<Dictionary<UInt32, Single>>	m_weights			= new List<Dictionary<UInt32, Single>>( );
		private readonly	List<Single>						m_timescale_levels	= new List<float>( );
		private readonly	List<Single>						m_weight_levels		= new List<float>( );


		#endregion

		#region |   Methods  |


		private		UInt32		find_next_f_event_position	( UInt32 start_position )	
		{
			var count = m_item.events.Count;
			for( var i = 0; i < count; ++i )
			{
				if ( m_item.events[i].m_position > start_position && m_item.events[i].text == "F" )
					return (UInt32)m_item.events[i].m_position;
			}
			return start_position + 100000;
		}

		private		Single		compute_weight				( Single value )			
		{
			return ( value - m_weights_min ) / ( m_weights_max - m_weights_min );
		}
		private		Single		compute_timescale			( Single value )			
		{
			return ( value - m_timescale_min ) / ( m_timescale_max - m_timescale_min );
		}
		private		void		compute_curves_extremums	( )							
		{
			foreach( var pair in m_item.weights_by_time )
			{
				if( pair.Value > m_weights_max ) m_weights_max = (Single)Math.Ceiling(pair.Value);
				if( pair.Value < m_weights_min ) m_weights_min = (Single)Math.Floor(pair.Value);
				m_weights_last_pos = pair.Key;
			}
			foreach( var pair in m_item.scales_by_time )
			{
				if( pair.Value > m_timescale_max ) m_timescale_max = (Single)Math.Ceiling(pair.Value);
				if( pair.Value < m_timescale_min ) m_timescale_min = (Single)Math.Floor(pair.Value);
				m_timescale_last_pos = pair.Key;
			}
		}
		private		void		create_optimized_curves		( )							
		{
			if( m_item.weights_by_time.Count > 1 )
			{
				var f_pos		= find_next_f_event_position( 0 );
				var work_dict	= new Dictionary<UInt32, Single>( );
				m_weights.Add		( work_dict );
				Single last_value = 0;

				foreach( var pair in m_item.weights_by_time )
				{
					var value = compute_weight( pair.Value );

					if( pair.Key == f_pos )
					{
						work_dict.Add	( pair.Key, value );
						work_dict		= new Dictionary<UInt32, Single>( );
						m_weights.Add		( work_dict );
						f_pos			= find_next_f_event_position( f_pos );
					}
					else if( pair.Key > f_pos )
					{
						work_dict.Add	( f_pos, last_value );

						work_dict		= new Dictionary<UInt32, Single>( );
						m_weights.Add		( work_dict );
						f_pos			= find_next_f_event_position( f_pos );
						work_dict.Add	( pair.Key, value );
					}
					else
						work_dict.Add	( pair.Key, value );

					last_value = value;
				}
			}
			
			if( m_item.scales_by_time.Count > 1 )
			{
				var f_pos			= find_next_f_event_position( 0 );
				var work_dict		= new Dictionary<UInt32, Single>( );
				m_scales.Add		( work_dict );
				Single last_value	= 0;

				foreach( var pair in m_item.scales_by_time )
				{
					var value	= compute_timescale( pair.Value );

					if( pair.Key == f_pos )
					{
						work_dict.Add	( pair.Key, value );
						work_dict		= new Dictionary<UInt32, Single>( );
						m_scales.Add	( work_dict );
						f_pos			= find_next_f_event_position( f_pos );
					}
					else if( pair.Key > f_pos )
					{
						work_dict.Add	( f_pos, last_value );

						work_dict		= new Dictionary<UInt32, Single>( );
						m_scales.Add		( work_dict );
						f_pos			= find_next_f_event_position( f_pos );
						work_dict.Add	( pair.Key, value );
					}
					else
						work_dict.Add	( pair.Key, value );

					last_value = value;
				}
			}

			for ( var i = (Int32)m_timescale_min; i < m_timescale_max; ++i )
				m_timescale_levels.Add( compute_timescale( i ) );

			for ( var i = (Int32)m_weights_min; i < m_weights_max; ++i )
				m_weight_levels.Add( compute_weight( i ) );

		}

		internal	void		fill_curves					( )							
		{
			m_weights_curves.Figures.Clear	( );
			m_scales_curves.Figures.Clear	( );

			if( m_item.weights_by_time.Count > 1 )
			{
				foreach( var anim_weights in m_weights )
				{
					var figure	= new PathFigure( );
					m_weights_curves.Figures.Add( figure );
					var is_first = true;

					foreach( var pair in anim_weights )
					{
						if( is_first )
						{
							is_first = false;
							figure.StartPoint = new Point( pair.Key * m_item.m_panel.time_layout_scale, ( 1 - pair.Value ) * m_item.height);
							continue;
						}

						figure.Segments.Add( new LineSegment( new Point( pair.Key * m_item.m_panel.time_layout_scale, ( 1 - pair.Value ) * m_item.height ), true ) );
					}
				}
			}

			if( m_item.scales_by_time.Count > 1 )
			{
			    foreach( var anim_scales in m_scales )
			    {
			        var figure	= new PathFigure( );
			        m_scales_curves.Figures.Add( figure );
					var is_first	= true;

			        foreach( var pair in anim_scales )
			        {
			            if( is_first )
			            {
			                is_first = false;
			                figure.StartPoint = new Point( pair.Key * m_item.m_panel.time_layout_scale, ( 1 - pair.Value ) * m_item.height);
			                continue;
			            }

			            figure.Segments.Add( new LineSegment( new Point( pair.Key * m_item.m_panel.time_layout_scale, ( 1 - pair.Value ) * m_item.height ), true ) );
			        }
			    }
			}

			foreach( var timescale_level in m_timescale_levels )
			{
				m_scales_levels.Figures.Add( 
					new PathFigure
					{ 
						StartPoint = new Point( 0, ( 1 - timescale_level ) * m_item.height ),
						Segments = { new LineSegment( new Point( m_timescale_last_pos, ( 1 - timescale_level ) * m_item.height ), true ) }
					} 
				);
			}
			foreach( var weight_level in m_weight_levels )
			{
				m_weights_levels.Figures.Add( 
					new PathFigure
					{ 
						StartPoint = new Point( 0, ( 1 - weight_level ) * m_item.height ),
						Segments = { new LineSegment( new Point( m_weights_last_pos, ( 1 - weight_level ) * m_item.height ), true ) }
					} 
				);
			}
		}


		#endregion

	}
}
