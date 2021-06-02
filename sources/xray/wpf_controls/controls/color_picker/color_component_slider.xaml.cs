////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace xray.editor.wpf_controls.color_picker
{
	/// <summary>
	/// Interaction logic for color_component_slider.xaml
	/// </summary>
	internal partial class color_component_slider
	{

		#region | Initialize |


		public				color_component_slider	( )					
		{
			InitializeComponent( );
		}


		#endregion

		#region |   Fields   |


		private				Border					m_background_border;
		private				LinearGradientBrush		m_gradient;
		private				Boolean					m_need_update_gradient;
		private				Boolean					m_is_value_setting;


		#endregion

		#region | Properties |


		public static		DependencyProperty			selected_color_property = DependencyProperty.Register( "selected_color", typeof(color_hsv), typeof(color_component_slider), new PropertyMetadata( (color_hsv)Colors.White, selected_color_changed ) ); 

		public static		Boolean						use_one_to_zero_range	
		{
			get;set;
		}
		public				color_hsv					selected_color			
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
		public				LinearGradientBrush			gradient				
		{
			get 
			{
				return m_gradient; 
			}
			set 
			{
				m_gradient				= value; 
				m_gradient.StartPoint	= new Point( 0.5, 0 );
				m_gradient.EndPoint		= new Point( 0.5, 1 );
				m_need_update_gradient	= true;
				update_gradient			( );
			}
		}
		public				String						title					
		{
			get 
			{
				return (String)m_title.Content; 
			}
			set 
			{
				m_title.Content = value;
			}
		}
		public				Single						maximum					
		{
			get
			{
				return (Single)m_slider.Maximum;
			}
			set
			{
				m_slider.Maximum = value;
			}
		}
		public				Single						minimum					
		{
			get
			{
				return (Single)m_slider.Minimum;
			}
			set
			{
				m_slider.Minimum = value;
			}
		}
		public				Single						value					
		{
			get
			{
				return (Single)m_slider.Value;
			}
			set
			{
				m_slider.Value = value;
			}
		}
		public				Single						small_change			
		{
			get
			{
				return (Single)m_slider.SmallChange;
			}
			set
			{
				m_slider.SmallChange = value;
			}
		}
		public				Single						large_change			
		{
			get
			{
				return (Single)m_slider.LargeChange;
			}
			set
			{
				m_slider.LargeChange = value;
			}
		}
		public				color_part_worker_base		worker					
		{
			get;set;
		}
		public				Boolean						use_hue_for_first_stop	
		{
			get;
			set;
		}


		#endregion

		#region |  Methods   |


		private static		void		selected_color_changed	( DependencyObject d, DependencyPropertyChangedEventArgs e )
		{
			var color_slider	= (color_component_slider)d;
			var new_color		= (color_hsv)e.NewValue;
			var new_value		= color_slider.worker.color_to_part( new_color );
			//color_slider.set_formatted_value( new_value );

			if( color_slider.worker != null && !color_slider.m_is_value_setting )
			{
				color_slider.m_is_value_setting = true;
				color_slider.m_slider.Value = new_value;
				if( color_slider.use_hue_for_first_stop )
					color_slider.m_gradient.GradientStops[0].Color = (Color)color_utilities.convert_hsv_to_rgb( new_color.h, 1, 1, 1 );

				color_slider.m_is_value_setting = false;
			}
		}
		private				void		update_gradient			( )															
		{
			if( m_need_update_gradient && m_background_border != null )
			{
				m_need_update_gradient			= false;
				m_background_border.Background	= m_gradient;
			}
		}
		private				void		slider_value_changed	( Object sender, RoutedPropertyChangedEventArgs<Double> e )	
		{
			set_formatted_value( e.NewValue );
			if( worker != null && !m_is_value_setting )
			{
				m_is_value_setting = true;
				selected_color = worker.part_to_color( m_slider.Value );
				m_is_value_setting = false;
			}
		}
		private				void		dock_panel_loaded		( Object sender, RoutedEventArgs e )						
		{
		    var obj					= m_slider.Template.FindName( "PART_TrackBackground", m_slider );
		    m_background_border		= obj as Border;
		    update_gradient			( );
		}
		private				void		set_formatted_value		( Double new_value )										
		{
			var str = use_one_to_zero_range ? String.Format( "{0:N}", new_value / m_slider.Maximum ): String.Format( "{0:N}", new_value );
			m_value.Text = str.TrimEnd('0').TrimEnd('.');
		}
		private				void		m_value_key_down		( Object sender, KeyEventArgs e )							
		{
			if( e.Key != Key.Enter )
				return;

			Single parse_result;
			if( Single.TryParse( m_value.Text, out parse_result ) )
				m_slider.Value = parse_result;
		}
		private				void		loast_focus				( Object sender, RoutedEventArgs e )							
		{
			Single parse_result;
			if (Single.TryParse(m_value.Text, out parse_result))
				m_slider.Value = parse_result;
		}


		#endregion

	}
}
