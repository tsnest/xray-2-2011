////////////////////////////////////////////////////////////////////////////
//	Created		: 29.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Windows;

namespace xray.editor.wpf_controls.color_picker
{
	/// <summary>
	/// Interaction logic for color_picker_dlg.xaml
	/// </summary>
	public partial class dialog
	{
		public				dialog	( )						
		{
			InitializeComponent();

			OKButton.IsEnabled				= false;
		}

		public				color_rgb		selected_color					
		{
			get
			{
				var color			= color_utilities.convert_hsv_to_rgb( m_color_picker.selected_color );
				color.multiply		( m_color_picker.m_multiply_slider.value );
				return color;
			}
			set
			{
				var multiply_factor						= value.multily_factor;
				value.multiply							( 1 / multiply_factor );
				m_color_picker.m_multiply_slider.value	= (Single)multiply_factor;
				m_color_picker.selected_color			= color_utilities.convert_rgb_to_hsv( value );
			}
		}

		private				void		ok_button_clicked				(Object sender, RoutedEventArgs e)							
		{
			OKButton.IsEnabled	= false;
			DialogResult		= true;
			Close				( );
		}
		private				void		cancel_button_clicked			(Object sender, RoutedEventArgs e)							
		{
			OKButton.IsEnabled	= false;
			DialogResult		= false;
		}
		private				void		on_selected_color_changed		(Object sender, RoutedPropertyChangedEventArgs<color_hsv> e)	
		{
			if ( e.NewValue != e.OldValue )
			{
				OKButton.IsEnabled = true;
			}
		}

		protected override	void		OnClosing						(CancelEventArgs e)											
		{
			OKButton.IsEnabled	= false;
			base.OnClosing		( e );
		}

	}
}
