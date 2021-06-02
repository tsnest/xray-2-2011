////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace xray.editor.wpf_controls.color_matrix_editor
{
	/// <summary>
	/// Interaction logic for color_range_panel.xaml
	/// </summary>
	public partial class color_matrix_control
	{

		#region | Initialize |


		public			color_matrix_control			( )				
		{
			InitializeComponent( );
			SetValue( Panel.ZIndexProperty, 50 );

			m_context_menu.Opened += context_menu_opened;

			m_gradient_rects_matrix = new sizeable_matrix<gradient_rect>( );
			m_control_points_matrix = new sizeable_matrix<control_point>( );

		}


		#endregion

		#region |   Fields   |


		private	const		Double									c_epsilon = 0.0001;

		private 			sizeable_matrix<control_point>			m_control_points_matrix;
		private				sizeable_matrix<gradient_rect>			m_gradient_rects_matrix;
		private				Point									m_context_menu_open_position;
		private				Int32									m_captured_point_x_index;
		private				Int32									m_captured_point_y_index;
		private				color_matrix							m_edited_color_matrix;

		private				Double									m_margin;


		#endregion

		#region | Properties |


		public				color_matrix		edited_color_matrix			
		{
			get
			{
				return m_edited_color_matrix;
			}
			set
			{
				m_edited_color_matrix		= value;
				recreate_range_from_matrix	( );
			}
		}
		public				Double				logical_width				
		{
			get
			{
				return m_edited_color_matrix.logical_size.Width;
			}
		}
		public				Double				logical_height				
		{
			get
			{
				return m_edited_color_matrix.logical_size.Height;
			}
		}
		public				Double				visual_width				
		{
			get
			{
				return ActualWidth - 2 * m_margin;
			}
		}
		public				Double				visual_height				
		{
			get
			{
				return ActualHeight - 2 * m_margin;
			}
		}

		public				Double				margin						
		{
			get
			{
				return m_margin;
			}
			set
			{
				m_margin						= value;
				m_work_panel.Margin				= new Thickness( value );
				fix_control_points_positions	( );
			}
		}


		#endregion

		#region |   Methods  |


		private				void				context_menu_opened			( Object sender, RoutedEventArgs e )			
		{
			m_context_menu_open_position = Mouse.PrimaryDevice.GetPosition( m_work_panel );
		}
		private				void				add_horizontal_click		( Object sender, RoutedEventArgs e )			
		{
			var y_pos				= m_context_menu_open_position.Y.to_logical_y( this );
			var y_index				= find_y_control_index( y_pos );
			
			//add_new_gradients_row
			var gradients_row = new List<gradient_rect>( m_gradient_rects_matrix.width );

			for( var i = 0; i < m_gradient_rects_matrix.width; ++i )
			{
				var rect = new gradient_rect();
				gradients_row.Add						( rect );
				m_work_panel.Children.Add	( rect );
			}

			m_gradient_rects_matrix.insert_row( y_index, gradients_row );
		
			//add_new_controls_row
			var row			= new List<control_point>( m_control_points_matrix.width );
			var color_row	= new List<color_matrix_element>( m_control_points_matrix.width );

			for( var i = 0; i < m_control_points_matrix.width; ++i )
			{
				var color_element = new color_matrix_element {
                	position = new Point( m_control_points_matrix[i, 0].position.X, m_context_menu_open_position.Y.to_logical_y( this ) )
                };
				var control = new control_point( this ){ color_matrix_element = color_element };
				
				relink_gradients_for		( m_control_points_matrix[i, y_index], i, y_index + 1 );
				relink_gradients_for		( control, i, y_index );

				control.set_color_from		( m_control_points_matrix[i, y_index - 1], m_control_points_matrix[i, y_index] );

				color_row.Add				( color_element );
				row.Add						( control );
				m_work_panel.Children.Add	( control );
			}

			m_control_points_matrix.insert_row	( y_index, row );
			m_edited_color_matrix.insert_row	( y_index, color_row );

			fix_control_points_positions		( );
			
			m_edited_color_matrix.invoke_edit_complete( );

		}
		private				void				add_vertical_click			( Object sender, RoutedEventArgs e )			
		{
			var x_pos = m_context_menu_open_position.X.to_logical_x( this );
			var x_index				= find_x_control_index( x_pos );

			//add_new_gradients_column
			var gradients_column = new List<gradient_rect>( m_gradient_rects_matrix.height );

			for( var i = 0; i < m_gradient_rects_matrix.height; ++i )
			{
				var rect = new gradient_rect	( );
				gradients_column.Add						( rect );
				m_work_panel.Children.Add		( rect );
			}

			m_gradient_rects_matrix.insert_column( x_index, gradients_column );

			//add_new_controls_column
			var column			= new List<control_point>( m_control_points_matrix.height );
			var color_column	= new List<color_matrix_element>( m_control_points_matrix.height );

			for( var i = 0; i < m_control_points_matrix.height; ++i )
			{
				var color_element = new color_matrix_element {
                	position = new Point( m_context_menu_open_position.X.to_logical_x( this ), m_control_points_matrix[0, i].position.Y )
                };
				var control = new control_point( this ){ color_matrix_element = color_element };

				relink_gradients_for		( m_control_points_matrix[ x_index, i], x_index + 1 , i );
				relink_gradients_for		( control, x_index, i );

				control.set_color_from		( m_control_points_matrix[x_index - 1, i], m_control_points_matrix[x_index , i] );

				column.Add					( control );
				color_column.Add			( color_element );
				m_work_panel.Children.Add	( control );
			}

			m_edited_color_matrix.insert_column		( x_index, color_column );
			m_control_points_matrix.insert_column	( x_index, column );

			fix_control_points_positions		( );

			m_edited_color_matrix.invoke_edit_complete( );
		}
		private				void				size_changed				( Object sender, SizeChangedEventArgs e )		
		{
			fix_control_points_positions( );
		}
	
		private				void				recreate_range_from_matrix	( )												
		{
			m_work_panel.Children.Clear		( );
			m_gradient_rects_matrix.clear	( );
			m_control_points_matrix.clear	( );

			var width	= m_edited_color_matrix.width;
			var height	= m_edited_color_matrix.height;

			//create rects
			m_gradient_rects_matrix = new sizeable_matrix<gradient_rect> ( width - 1, height - 1 );

			for( var i = 1; i < height; i++ )
			{
				for( var j = 1; j < width; j++ )
				{
					var rect								= new gradient_rect( );
					m_gradient_rects_matrix[j - 1, i - 1]	= rect;
					m_work_panel.Children.Add				( rect );
				}
			}

			//create control points
			m_control_points_matrix = new sizeable_matrix<control_point> ( width, height );

			for( var i = 0; i < height; i++ )
			{
				for( var j = 0; j < width; j++ )
				{
					var point						= new control_point( this )
					{ 
                   		color_matrix_element = m_edited_color_matrix[j, i]
					};
					m_control_points_matrix[j, i]	= point;
					m_work_panel.Children.Add		( point );
					relink_gradients_for			( point, j, i );
					point.apply_position			( );
				}
			}

			fix_control_points_positions	( );
			
		}
		
		private				void				relink_gradients_for		( control_point control, Int32 x, Int32 y )		
		{
			control.rect_1 = null;
			control.rect_2 = null;
			control.rect_3 = null;
			control.rect_4 = null;

			if( m_gradient_rects_matrix.is_item_exists( x, y ) )
				control.rect_3 = m_gradient_rects_matrix[x, y];

			if( m_gradient_rects_matrix.is_item_exists( x, y - 1 ) )
				control.rect_2 = m_gradient_rects_matrix[x, y - 1];

			if( m_gradient_rects_matrix.is_item_exists( x - 1, y - 1 ) )
				control.rect_1 = m_gradient_rects_matrix[x - 1, y - 1];

			if( m_gradient_rects_matrix.is_item_exists( x - 1, y ) )
				control.rect_4 = m_gradient_rects_matrix[x - 1, y];

			control.apply_color		( );
			control.apply_position	( );
		}

		private				Int32				find_x_control_index		( Double x_position )							
		{
			for( var i = 0; i < m_control_points_matrix.width; ++i )
			{
				if( m_control_points_matrix[i, 0].position.X < x_position )
					continue;

				return i;
			}
			
			return -1;
		}
		private				Int32				find_y_control_index		( Double y_position )							
		{
			for( var j = 0; j < m_control_points_matrix.height; ++j )
			{
				if( m_control_points_matrix[0, j].position.Y < y_position )
					continue;

				return j;
			}

			return -1;
		}

		internal			void				remove_horizontal			( Double y_position )							
		{
			var y_index									= find_y_control_index( y_position );
			
			//remove_gradients_row
			for( var i = 0; i < m_gradient_rects_matrix.width; ++i )
			{
				var rect											= m_gradient_rects_matrix[i, y_index];
				m_work_panel.Children.Remove						( rect );
				m_gradient_rects_matrix[i, y_index - 1].right_y		= rect.right_y;
			}

			m_gradient_rects_matrix.remove_row_at		( y_index );

			//remove_controls_row
			for( var i = 0; i < m_control_points_matrix.width; ++i )
			{
				var control											= m_control_points_matrix[i, y_index];
				relink_gradients_for								( m_control_points_matrix[i, y_index + 1], i, y_index );
				m_work_panel.Children.Remove						( control );
			}

			m_edited_color_matrix.remove_row_at			( y_index );
			m_control_points_matrix.remove_row_at		( y_index );

			m_edited_color_matrix.invoke_edit_complete	( );
		}
		internal			void				remove_vertical				( Double x_position )							
		{
			var x_index				= find_x_control_index( x_position );

			//remove_gradients_column
			for( var i = 0; i < m_gradient_rects_matrix.height; ++i )
			{
				var rect											= m_gradient_rects_matrix[x_index, i];
				m_work_panel.Children.Remove						( rect );
				m_gradient_rects_matrix[x_index - 1, i].right_x		= rect.right_x;
			}

			m_gradient_rects_matrix.remove_column_at( x_index );
		
			//remove_controls_column
			for( var i = 0; i < m_control_points_matrix.height; ++i )
			{
				var control						= m_control_points_matrix[x_index, i];
				relink_gradients_for			( m_control_points_matrix[x_index + 1, i], x_index, i );
				m_work_panel.Children.Remove	( control );
			}

			m_edited_color_matrix.remove_column_at		( x_index );
			m_control_points_matrix.remove_column_at	( x_index );

			m_edited_color_matrix.invoke_edit_complete( );
		}
		internal			void				move_horizontal_line		( )												
		{
			var new_pos = Mouse.GetPosition( m_work_panel );
			new_pos = new Point( new_pos.X / visual_width * logical_width, new_pos.Y / visual_height * logical_height );

			if( new_pos.Y > m_control_points_matrix[ 0, m_captured_point_y_index + 1 ].y )
				new_pos.Y = m_control_points_matrix[ 0, m_captured_point_y_index + 1 ].y - c_epsilon;

			if( new_pos.Y < m_control_points_matrix[ 0, m_captured_point_y_index - 1 ].y )
				new_pos.Y = m_control_points_matrix[ 0, m_captured_point_y_index - 1 ].y + c_epsilon;

			for( var i = 0; i < m_control_points_matrix.width; ++i )
				m_control_points_matrix[i, m_captured_point_y_index ].y		= new_pos.Y;

		}
		internal			void				move_vertical_line			( )												
		{
			var new_pos = Mouse.GetPosition( m_work_panel );
			new_pos = new Point( new_pos.X / visual_width * logical_width, new_pos.Y / visual_height * logical_height );

			if( new_pos.X > m_control_points_matrix[ m_captured_point_x_index + 1 , 0 ].x )
				new_pos.X = m_control_points_matrix[ m_captured_point_x_index + 1 , 0 ].x - c_epsilon;

			if( new_pos.X < m_control_points_matrix[ m_captured_point_x_index - 1 , 0 ].x )
				new_pos.X = m_control_points_matrix[ m_captured_point_x_index - 1 , 0 ].x + c_epsilon;

			for( var i = 0; i < m_control_points_matrix.height; ++i )
				m_control_points_matrix[ m_captured_point_x_index, i ].x	= new_pos.X;

		}
		internal			void				fix_control_point_indexes	( control_point point )							
		{
			m_captured_point_x_index = find_x_control_index( point.x );
			m_captured_point_y_index = find_y_control_index( point.y );
		}
		internal			void				invoke_edit_complete		( )												
		{
			m_edited_color_matrix.invoke_edit_complete( );
		}
		internal			void				fix_control_points_positions( )												
		{
		    for( var i = 0; i < m_control_points_matrix.width; ++i )
			{
				for( var j = 0; j < m_control_points_matrix.height; ++j )
				{
					m_control_points_matrix[i, j].apply_position( );
				}
			}
		}
		

		#endregion

	}
	internal static class color_range_control_extensions	
	{
		internal static		Double				to_logical_x			( this Double x , color_matrix_control ctrl )		
		{
			return x / ctrl.visual_width * ctrl.logical_width;
		}
		internal static		Double				to_logical_y			( this Double y , color_matrix_control ctrl )		
		{
			return y / ctrl.visual_height * ctrl.logical_height;
		}
		internal static		Double				to_visual_x				( this Double x , color_matrix_control ctrl )		
		{
			return x / ctrl.logical_width * ctrl.visual_width;
		}
		internal static		Double				to_visual_y				( this Double y , color_matrix_control ctrl )		
		{
			return y / ctrl.logical_height * ctrl.visual_height;
		}
		internal static		Point				to_logical				( this Point point , color_matrix_control ctrl )	
		{
			return new Point( point.X / ctrl.visual_width * ctrl.logical_width, point.Y / ctrl.visual_height * ctrl.logical_height );
		}
		internal static		Point				to_visual				( this Point point , color_matrix_control ctrl )	
		{
			return new Point( point.X / ctrl.logical_width * ctrl.visual_width, point.Y / ctrl.logical_height * ctrl.visual_height );
		}
	}
}
