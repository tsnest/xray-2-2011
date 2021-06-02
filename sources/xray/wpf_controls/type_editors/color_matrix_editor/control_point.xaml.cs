////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using xray.editor.wpf_controls.color_picker;
using xray.editor.wpf_controls.color_range;

namespace xray.editor.wpf_controls.color_matrix_editor
{
	/// <summary>
	/// Interaction logic for control_point.xaml
	/// </summary>
	public partial class control_point
	{

		#region | Initialize |


		public control_point	( color_matrix_control owner )		
		{
			this.owner			= owner;
			InitializeComponent	( );
			SetValue			( Panel.ZIndexProperty, 100 );
		}


		#endregion

		#region |   Fields   |


		private		Boolean					m_is_captured;
		private		color_matrix_element	m_color_matrix_element;
		private		Point					m_visual_position;


		#endregion

		#region | Properties |


		public		Boolean					is_corner_point				
		{
			get
			{
				return is_vertical_border_point && is_horizontal_border_point;
			}
		}
		public		Boolean					is_vertical_border_point	
		{
			get
			{
				return ( rect_1 == null && rect_2 == null ) || ( rect_3 == null && rect_4 == null );
			}
		}
		public		Boolean					is_horizontal_border_point	
		{
			get
			{
				return ( rect_1 == null && rect_4 == null ) || ( rect_2 == null && rect_3 == null );
			}
		}

		public		Double					x							
		{
			get
			{
				return m_color_matrix_element.position.X;
			}
			set
			{
				m_color_matrix_element.position.X = value;
				m_visual_position.X = x / owner.logical_width * owner.visual_width;
				SetValue			( Canvas.LeftProperty, m_visual_position.X - Width / 2 );
				set_rects_positions	( );
			}
		}
		public		Double					y							
		{
			get
			{
				return m_color_matrix_element.position.Y;
			}
			set
			{
				m_color_matrix_element.position.Y = value;
				m_visual_position.Y = y / owner.logical_height * owner.visual_height;
				SetValue			( Canvas.TopProperty, m_visual_position.Y - Height / 2 );
				set_rects_positions	( );
			}
		}
		public		Point					position					
		{
			get
			{
				return new Point( x, y );
			}
			set
			{
				x = value.X;
				y = value.Y;
			}
		}

		public		gradient_rect			rect_1						
		{
			get;set;
		}
		public		gradient_rect			rect_2						
		{
			get;set;
		}
		public		gradient_rect			rect_3						
		{
			get;set;
		}
		public		gradient_rect			rect_4						
		{
			get;set;
		}

		public		color_matrix_element	color_matrix_element		
		{
			get
			{
				return m_color_matrix_element;
			}
			set
			{
				m_color_matrix_element = value;
				update_position_from_element( );
			}
		}
		public		color_matrix_control	owner						
		{
			get;
			private set;
		}
		public		color_rgb				color						
		{
			get
			{
				return m_color_matrix_element.color;
			}
			set
			{
				m_color_matrix_element.color = value;

				if( rect_1 != null )
					rect_1.color_bottom_right	= value;
				if( rect_2 != null )
					rect_2.color_bottom_left	= value;
				if( rect_3 != null )
					rect_3.color_top_left		= value;
				if( rect_4 != null )
					rect_4.color_top_right		= value;
			}
		}


		#endregion

		#region |   Methods  |


		private		void			context_menu_opened				( Object sender, RoutedEventArgs e )			
		{
			m_remove_vertical.IsEnabled		= !is_horizontal_border_point;
			m_remove_horizontal.IsEnabled	= !is_vertical_border_point;
		}
		private		void			update_position_from_element	( )												
		{
			SetValue			( Canvas.LeftProperty, x - Width / 2 );
			SetValue			( Canvas.TopProperty, y - Height / 2 );
		}

		private		void			mouse_double_click				( Object sender, MouseButtonEventArgs e )		
		{
			var color_dlg		= new dialog { selected_color = color, Owner = Window.GetWindow( this ) };

			if( color_dlg.ShowDialog( ) == true )
			{
				color = color_dlg.selected_color;
				owner.invoke_edit_complete( );
			}

			e.Handled = true;
		}
		private		void			remove_horizontal_click			( Object sender, RoutedEventArgs e )			
		{
			owner.remove_horizontal	( position.Y );
		}
		private		void			remove_vertical_click			( Object sender, RoutedEventArgs e )			
		{
			owner.remove_vertical	( position.X );
		}

		private		void			mouse_down						( Object sender, MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Left )
			{
				e.Handled						= true;
				CaptureMouse					( );
				owner.fix_control_point_indexes	( this );
				m_is_captured					= true;
			}

		}
		private		void			mouse_move						( Object sender, MouseEventArgs e )				
		{
			if( m_is_captured )
			{
				if( ( rect_1 != null && rect_4 != null ) || ( rect_2 != null && rect_3 != null ) )
					owner.move_horizontal_line( );
				if( ( rect_1 != null && rect_2 != null ) || ( rect_4 != null && rect_3 != null ) )
					owner.move_vertical_line( );
			}
		}
		private		void			mouse_up						( Object sender, MouseButtonEventArgs e )		
		{
			if( m_is_captured )
			{
				m_is_captured				= false;
				ReleaseMouseCapture			( );
				owner.invoke_edit_complete	( );
			}
		}

		public		void			set_color_from					( control_point first, control_point second )	
		{
			if( first.position.X == second.position.X )
			{
				var length	= Math.Abs( first.position.Y - second.position.Y );
				var delta	= Math.Abs( position.Y - first.position.Y );

				color		= color_rgb.learp( first.color, second.color, delta / length );
			}
			else
			{
				var length	= Math.Abs( first.position.X - second.position.X );
				var delta	= Math.Abs( position.X - first.position.X );

				color		= color_rgb.learp( first.color, second.color, delta / length );
			}
		}
		public		void			set_rects_positions				( )												
		{
			if( rect_1 != null )
				rect_1.bottom_right	= m_visual_position;
			if( rect_2 != null )
				rect_2.bottom_left	= m_visual_position;
			if( rect_3 != null )
				rect_3.top_left		= m_visual_position;
			if( rect_4 != null )
				rect_4.top_right	= m_visual_position;
		}
		public		void			apply_position					( )												
		{
			position = position;
		}
		public		void			apply_color						( )												
		{
			color = color;
		}


		#endregion

	}
}
