////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.color_matrix_editor;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for color_matrix_editor.xaml
	/// </summary>
	public partial class color_matrix_editor
	{
		public			color_matrix_editor			( )		
		{
			InitializeComponent( );

			DataContextChanged += delegate
			{
				m_property = (property)DataContext;

				if( m_property == null )
					return;

				m_color_matrix_editor.edited_color_matrix = (color_matrix)m_property.value;
			};

		}

		private			void		big_editor_click		( Object sender, RoutedEventArgs e )		
		{
			var color_matrix_editor		= new color_matrix_control { margin = 10 };
			var border					= new Border( );
			border.SetValue				( PaddingProperty, new Thickness( 4 ) );
			border.Child				= color_matrix_editor;
			m_color_matrix_editor.IsEnabled = false;

			var window					= new Window { Content = border, Width = 800, Height = 600, Title = "Color Matrix Editor" };
			color_matrix_editor.edited_color_matrix = (color_matrix)m_property.value;
			window.Loaded += delegate
			{ 
				color_matrix_editor.fix_control_points_positions( ); 
			};
			window.Closed += delegate 
			{
				window.Content								= null;
				m_color_matrix_editor.edited_color_matrix	= color_matrix_editor.edited_color_matrix;
				m_color_matrix_editor.IsEnabled				= true;
			};
			window.Show	( );
		}
		public override void		update					( )											
		{
			m_color_matrix_editor.edited_color_matrix = (color_matrix)m_property.value;
		}
	}
}
