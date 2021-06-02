////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;

namespace xray.editor.wpf_controls.color_picker
{
	/// <summary>
	/// Interaction logic for color_picker_hsb.xaml
	/// </summary>
	public partial class color_palette
	{

		#region | Initialize |


		public			color_palette	( )		
		{
			InitializeComponent( );
		}
		internal static	void initialize		( )		
		{
			m_hue_gradient	= new LinearGradientBrush { StartPoint = new Point( 0.5, 0 ), EndPoint = new Point( 0.5, 1 ) };
			var hsb_colors	= color_utilities.generate_hsb_spectrum( );
			var count		= hsb_colors.Count;

			for( var i = 0; i < count; ++i )
				m_hue_gradient.GradientStops.Add( new GradientStop ( hsb_colors[i], (Double)i / count ) );

			selected_color_changed_event	= EventManager.RegisterRoutedEvent(
				"selected_color_changed", RoutingStrategy.Bubble, typeof(RoutedPropertyChangedEventHandler<color_hsv>), typeof(color_palette)
				);

			selected_color_property = DependencyProperty.Register( "selected_color", typeof(color_hsv), typeof(color_palette), new PropertyMetadata( (color_hsv)Colors.White , selected_color_property_changed ) );
		}


		#endregion

		#region |   Fields   |


		private static		LinearGradientBrush		m_hue_gradient;


		#endregion

		#region | Properties |


		public static	LinearGradientBrush		hue_gradient				
		{
			get
			{
				return m_hue_gradient;
			}
		}
		public static	DependencyProperty		selected_color_property;
		public			color_hsv					selected_color				
		{
			get
			{
				return (color_hsv)GetValue( selected_color_property );
			}
			set
			{
				SetValue( selected_color_property, value );
				m_startup_color_border.Background = new SolidColorBrush( (Color)value );
			}
		}


		#endregion

		#region |   Events   |


		public event				RoutedPropertyChangedEventHandler<color_hsv>	selected_color_changed		
		{
			add
			{
				AddHandler(selected_color_changed_event, value);
			}

			remove
			{
				RemoveHandler(selected_color_changed_event, value);
			}
		}
		public static				RoutedEvent									selected_color_changed_event;


		#endregion


		private static	void		selected_color_property_changed		( DependencyObject obj, DependencyPropertyChangedEventArgs e )	
		{
			var picker = (color_palette)obj;
			picker.RaiseEvent( new RoutedPropertyChangedEventArgs<color_hsv>( (color_hsv)e.OldValue, (color_hsv)e.NewValue, selected_color_changed_event ) );
		}

		private			void		startup_color_border_mouse_down		( Object sender, MouseButtonEventArgs e )		
		{
			selected_color = (color_hsv)( (SolidColorBrush)m_startup_color_border.Background ).Color;
		}

		private			void		rgb_button_click					( Object sender, RoutedEventArgs e )			
		{
			if( r_slider.Visibility == Visibility.Visible )
			{
				r_slider.Visibility = Visibility.Collapsed;
				g_slider.Visibility = Visibility.Collapsed;
				b_slider.Visibility = Visibility.Collapsed;
			}
			else
			{
				r_slider.Visibility = Visibility.Visible;
				g_slider.Visibility = Visibility.Visible;
				b_slider.Visibility = Visibility.Visible;
			}
		}

		private			void		hsv_button_click					( Object sender, RoutedEventArgs e )			
		{
			if( h_slider.Visibility == Visibility.Visible )
			{
				h_slider.Visibility = Visibility.Collapsed;
				s_slider.Visibility = Visibility.Collapsed;
				v_slider.Visibility = Visibility.Collapsed;
			}
			else
			{
				h_slider.Visibility = Visibility.Visible;
				s_slider.Visibility = Visibility.Visible;
				v_slider.Visibility = Visibility.Visible;
			}
		}
	}
}
