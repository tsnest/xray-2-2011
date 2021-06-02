////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace xray.editor.wpf_controls.color_picker
{
	/// <summary>
	/// Interaction logic for hsb_circle.xaml
	/// </summary>
	internal partial class hsv_circle
	{

		#region | Initialize |


		public			hsv_circle	( )		
		{
			InitializeComponent();
		}


		#endregion

		#region | Properties |


		public static readonly	DependencyProperty		selected_color_property = DependencyProperty.Register( "selected_color", typeof(color_hsv), typeof(hsv_circle), new FrameworkPropertyMetadata( (color_hsv)Colors.White, new PropertyChangedCallback( selected_color_changed_callback ) ) );
		public					color_hsv				selected_color		
		{
			get
			{
				return (color_hsv)GetValue( selected_color_property );
			}
			set
			{
				SetValue( selected_color_property, value );
			}
		}


		#endregion

		#region |   Events   |


		//public event				RoutedPropertyChangedEventHandler<Color>	selected_color_changed				
		//{
		//    add
		//    {
		//        AddHandler(selected_color_changed_event, value);
		//    }

		//    remove
		//    {
		//        RemoveHandler(selected_color_changed_event, value);
		//    }
		//}
		//public static readonly		RoutedEvent									selected_color_changed_event;


		#endregion

		#region |  Methods   |


		private static	void		selected_color_changed_callback		( DependencyObject prop, DependencyPropertyChangedEventArgs e )
		{
			var hsv = (color_hsv)e.NewValue;
			( (hsv_circle)prop ).m_hue_spectrum.hue = hsv.h;
			( (hsv_circle)prop ).compute_marker_position( );
			( (hsv_circle)prop ).m_triangle_hue_color.Color	= (Color)color_utilities.convert_hsv_to_rgb( hsv.h, 1, 1, hsv.a );
		}

		private			void		saturation_brightness_mouse_down	( Object sender, MouseButtonEventArgs e )	
		{
			compute_new_sat_bri								( );	
			m_saturation_brightness_spectrum.CaptureMouse	( );
		}
		private			void		saturation_brightness_mouse_move	( Object sender, MouseEventArgs e )			
		{
			if( e.LeftButton == MouseButtonState.Pressed )
				compute_new_sat_bri	( );	
		}
		private			void		saturation_brightness_mouse_up		( Object sender, MouseButtonEventArgs e )	
		{
			m_saturation_brightness_spectrum.ReleaseMouseCapture( );
		}
		private			void		m_hue_spectrum_hue_changed			( )											
		{
			var hsv_color				= selected_color;
			hsv_color.h					= m_hue_spectrum.hue;
			selected_color				= hsv_color;
		}

		private			void		compute_new_sat_bri					( )											
		{
			var pt									= Mouse.PrimaryDevice.GetPosition( m_saturation_brightness_spectrum );
			fix_position							( ref pt );
			//m_saturation_brightness_marker.Margin	= new Thickness( pt.X, pt.Y, 0, 0 );
			compute_new_color						( pt );
		}
		private			void		compute_new_color					( Point pick_position )						
		{
			var hsv_color		= selected_color;
			hsv_color.v			= 1 - pick_position.Y / m_saturation_brightness_spectrum.ActualHeight;

			var x_middle		= m_saturation_brightness_spectrum.ActualWidth / 2;

			var left			= x_middle - x_middle * hsv_color.v;
			var right			= x_middle + x_middle * hsv_color.v;
			var delta			= right - left;
			hsv_color.s			= ( delta == 0 ) ? 0 : ( pick_position.X - left ) / ( delta );
			
			selected_color		= hsv_color;
		}
		private			void		fix_position						( ref Point point )							
		{
			if( point.Y < 0 ) point.Y = 0;
			if( point.Y > m_saturation_brightness_spectrum.ActualHeight ) point.Y = m_saturation_brightness_spectrum.ActualHeight;

			var v_position	= 1 - point.Y / m_saturation_brightness_spectrum.ActualHeight;
			var x_middle	= m_saturation_brightness_spectrum.ActualWidth / 2;

			var left		= x_middle - x_middle * v_position;
			var right		= x_middle + x_middle * v_position;

			if( point.X < left ) point.X = left;
			if( point.X > right) point.X = right;
		}
		private			void		compute_marker_position				( )											
		{
			var new_marker_pt	= new Point( 0, 0 );

			var hsv_color		= selected_color;
			
			new_marker_pt.Y		= ( 1 - hsv_color.v ) * m_saturation_brightness_spectrum.Height;

			var x_middle		= m_saturation_brightness_spectrum.Width / 2;

			var left			= x_middle - x_middle * hsv_color.v;
			var right			= x_middle + x_middle * hsv_color.v;

			new_marker_pt.X		= hsv_color.s * ( right - left ) + left;

			m_saturation_brightness_marker.Margin	= new Thickness( new_marker_pt.X, new_marker_pt.Y, 0, 0 );
		}


		#endregion

	}
}
