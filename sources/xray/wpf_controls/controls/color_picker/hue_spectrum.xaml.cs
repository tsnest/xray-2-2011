////////////////////////////////////////////////////////////////////////////
//	Created		: 29.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Input;

namespace xray.editor.wpf_controls.color_picker
{
	/// <summary>
	/// Interaction logic for hue_spectrum.xaml
	/// </summary>
	internal partial class hue_spectrum
	{
		public hue_spectrum( )
		{
			InitializeComponent();
		}

		#region |   Events   |


		public event		Action		hue_changed;


		#endregion

		private const	Int32					c_spector_width = 16;
		private double m_hue;

		public			Double					hue
		{
			get 
			{
				return m_hue; 
			}
			set 
			{
				m_hue						= value; 
				move_marker_to_hue_position	( );
				on_hue_changed				( );
			}
		}

		private			void		spector_mouse_down				( Object sender, MouseButtonEventArgs e )					
		{
			var pt = fix_position	( e.GetPosition( m_hue_spectrum ) );
			m_hue_marker.Margin		= new Thickness( pt.X + m_hue_spectrum.ActualWidth / 2, pt.Y + m_hue_spectrum.ActualHeight / 2, 0, 0 );
			pic_color_in_position	( pt );
			m_hue_spectrum.CaptureMouse( );
		}
		private			void		spector_mouse_move				( Object sender, MouseEventArgs e )							
		{
			if( Mouse.PrimaryDevice.LeftButton == MouseButtonState.Pressed )
			{
				var pt = fix_position	( e.GetPosition( m_hue_spectrum ) );
				pic_color_in_position	( pt );
			}
		}
		private			void		spector_mouse_up				( Object sender, MouseButtonEventArgs e)					
		{
			m_hue_spectrum.ReleaseMouseCapture( );
		}
		private			Point		fix_position					( Point point )												
		{
			var center_point		= new Point( m_hue_spectrum.ActualWidth / 2, m_hue_spectrum.ActualHeight / 2 );
			var direction_vector	= point - center_point;

			direction_vector.Normalize( );
			direction_vector *= m_hue_spectrum.ActualWidth / 2 - c_spector_width;

			return (Point)direction_vector;
		}
		private			void		pic_color_in_position			( Point position )											
		{
			var degree		= Math.Atan2( position.Y, position.X ) / Math.PI * 180 + 180;
			degree			= ( degree + 325 ) % 360;
			hue				= degree;
		}
		private			void		move_marker_to_hue_position		( )												
		{
			var degree				= 360 - ( ( m_hue - 235 ) % 360 );
			var radians				= degree / 180 * Math.PI;
			var direction_vector	= new Vector( Math.Sin( radians ), Math.Cos( radians ) );
			direction_vector		*= Width / 2 - c_spector_width;
			m_hue_marker.Margin		= new Thickness( direction_vector.X + Width / 2, direction_vector.Y + Height / 2, 0, 0 );
		}
		

		protected		void		on_hue_changed					( )					
		{
		    if( hue_changed != null )
				hue_changed( );
		}

	}
}
