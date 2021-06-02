////////////////////////////////////////////////////////////////////////////
//	Created		: 21.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Media;

namespace xray.editor.wpf_controls.curve_editor.effects
{
	internal class visual_noise_effect: visual_effect_base
	{
		public visual_noise_effect( visual_curve parent_curve, float_curve_noise_effect noise ) : base ( parent_curve )
		{
			m_noise_effect = noise;

			m_random	= new Random( 32 );
			m_points	= new List<Point>( );

			var path_geomerty	= new PathGeometry( );
			m_path_figure		= new PathFigure( );

			path_geomerty.Figures.Add( m_path_figure );
			m_effect_curve.Data = path_geomerty;

			m_parent_curve.key_changed	+= curve_key_changed;
			m_parent_curve.key_added	+= key_added;
			m_parent_curve.key_removed	+= key_removed;

			generate_effect_curve( );

			m_parent_curve.effects.Add( this );
			m_parent_curve.float_curve.effects.Add( m_noise_effect );

			parent_curve.IsVisibleChanged += ( o, e )=> m_effect_curve.Visibility = m_parent_curve.Visibility;
		}

		private				float_curve_noise_effect	m_noise_effect;
		private				Random						m_random;
		private				PathFigure					m_path_figure;
		public				List<Point>					m_points;

		public				Int32			seed					
		{
			get 
			{
				return m_noise_effect.m_seed; 
			}
			set 
			{
				m_noise_effect.m_seed = value; 
				generate_effect_curve( );
			}
		}
		public				Single			frequency				
		{
			get 
			{
				return m_noise_effect.m_frequency; 
			}
			set 
			{ 
				m_noise_effect.m_frequency = value;
				generate_effect_curve( );
			}
		}
		public				Single			strength				
		{
			get 
			{
				return m_noise_effect.m_strength; 
			}
			set 
			{
				m_noise_effect.m_strength = value;
				generate_effect_curve( );
			}
		}
		public				Single			fade_in					
		{
			get 
			{
				return m_noise_effect.m_fade_in; 
			}
			set 
			{
				m_noise_effect.m_fade_in = value; 
				generate_effect_curve( );
			}
		}
		public				Single			fade_out				
		{
			get 
			{
				return m_noise_effect.m_fade_out; 
			}
			set 
			{
				m_noise_effect.m_fade_out = value; 
				generate_effect_curve( );
			}
		}

		private				void			curve_key_changed		( Int32 key_index )		
		{
			generate_effect_curve( );
		}
		private				void			key_removed				( visual_curve_key obj )
		{
			generate_effect_curve( );
		}
		private				void			key_added				( visual_curve_key obj )
		{
			generate_effect_curve( );
		}

		private				void			generate_effect_curve	( )						
		{
			m_points.Clear	( );
			m_random		= new Random( seed );

			var start_x			= m_parent_curve.keys[0].position_x;
			var end_x			= m_parent_curve.keys[m_parent_curve.keys.Count - 1].position_x;

			var step			= 1 / frequency;
			var local_fade_in	= 1 / m_noise_effect.m_fade_in;
			var local_fade_out	= 1 / m_noise_effect.m_fade_out;

			for( var x = start_x; x < end_x; x += step )
			{
				var fade		= 1.0;

				if( x - start_x < m_noise_effect.m_fade_in )
				{
					var t	= ( x - start_x ) * local_fade_in;
					fade	= 3*t*t - 2*t*t*t;
				}
				else if( end_x - x < m_noise_effect.m_fade_out )
				{
					var t	= ( end_x - x ) * local_fade_out;
					fade	= 3*t*t - 2*t*t*t;
				}

				var point		= m_evaluator.evaluate( x );
				var delta		= ( m_random.NextDouble( ) * 2 - 1 ) * ( strength * fade );
				point.Y			+= delta;
				m_points.Add	( point );
			}
			m_points.Add( m_evaluator.evaluate( end_x ) );
			generate_effect_path	( );
		}
		private				void			generate_effect_path	( )						
		{
			m_path_figure.Segments.Clear( );

			var last_visual_point		= m_parent_curve.logical_to_visual_point( m_points[0] );
			m_path_figure.StartPoint	= last_visual_point;

			var count = m_points.Count;
			for( var i = 1; i < count; ++i )
			{
				var curr_visual_point			= m_parent_curve.logical_to_visual_point( m_points[i] );
				m_path_figure.Segments.Add		( new BezierSegment( last_visual_point, curr_visual_point, curr_visual_point, true ) );
				last_visual_point				= curr_visual_point;
			}
		}

		internal override	void			update_visual			( )						
		{
			var last_visual_point		= m_parent_curve.logical_to_visual_point( m_points[0] );
			m_path_figure.StartPoint	= last_visual_point;

			var count = m_points.Count;
			for( var i = 1; i < count; ++i )
			{
				var curr_visual_point			= m_parent_curve.logical_to_visual_point( m_points[i] );
				var segment						= (BezierSegment )m_path_figure.Segments[i - 1];
				segment.Point1					= last_visual_point;
				segment.Point2					= curr_visual_point;
				segment.Point3					= curr_visual_point;

				last_visual_point				= curr_visual_point;
			}
		}
		internal override	void			open_effect_settings	( Window get_window )	
		{
			var form				= utils.new_form_host( "Noise Effect Settings", new noise_effect_settings_window{ effects = new List<visual_noise_effect> { this } } );
			form.StartPosition		= FormStartPosition.CenterParent;
			form.Owner				= Form.ActiveForm;
			form.Width				= 220;
			form.Height				= 160;
			form.FormBorderStyle	= FormBorderStyle.FixedToolWindow;
			form.ShowInTaskbar		= false;
			form.TopMost			= true;

			form.Show( );
		}
		internal override	void			open_effect_settings	( Window get_window, List<visual_effect_base> effects )	
		{
			var form				= utils.new_form_host( "Noise Effect Settings", new noise_effect_settings_window{ effects = effects.Cast<visual_noise_effect>( ).ToList( ) } );
			form.StartPosition		= FormStartPosition.CenterParent;
			form.Owner				= Form.ActiveForm;
			form.Width				= 220;
			form.Height				= 160;
			form.FormBorderStyle	= FormBorderStyle.FixedToolWindow;
			form.ShowInTaskbar		= false;
			form.TopMost			= true;

			form.Show( );
		}
		internal override	void			dispose					( )						
		{
			m_parent_curve.float_curve.effects.Remove( m_noise_effect );
			m_parent_curve.key_changed		-= curve_key_changed;
			m_parent_curve.effects.Remove	( this );

			base.dispose( );
		}
	}
}