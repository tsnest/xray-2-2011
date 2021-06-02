////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for color_ramp.xaml
	/// </summary>
	public partial class color_ramp
	{

		#region | Initialize |


		public color_ramp( )
		{
			InitializeComponent( );

			m_gradient				= (LinearGradientBrush)m_gradient_box.Background;
			
			m_add_context_menu		= new ContextMenu( );
			var add_item			= new MenuItem{ Header="Add Key" };
			add_item.Click			+= add_item_click;
			m_add_context_menu.Items.Add	( add_item );

			m_remove_context_menu	= new ContextMenu();
			var remove_item			= new MenuItem{ Header="Remove Key" };
			remove_item.Click		+= remove_item_click;
			m_remove_context_menu.Items.Add	( remove_item );
		}


		#endregion

		#region |   Events   |


		public		event	EventHandler<key_event_args>	key_added;
		public		event	EventHandler<key_event_args>	key_removed;
		public		event	EventHandler<key_event_args>	key_modified;
		public		event	EventHandler<key_event_args>	key_modification_start;
		public		event	EventHandler<key_event_args>	key_modification_done;


		#endregion

		#region |   Fields   |


		private readonly	ContextMenu				m_add_context_menu;
		private readonly	ContextMenu				m_remove_context_menu;

		private				LinearGradientBrush		m_gradient;
		private				Boolean					m_stop_cached;
		private				Point					m_last_mouse_position;

		private				Dictionary<GradientStop, color_rgb>		m_colors_dict = new Dictionary<GradientStop, color_rgb>( );

		internal const		Double					c_key_half_size = 2;


		#endregion

		#region | Properties |


		public		LinearGradientBrush		gradient
		{
			get { return m_gradient;}
			set { m_gradient = value;}
		}


		#endregion

		#region |   Methods  |


		private void			gradient_box_mouse_down					( Object sender, MouseButtonEventArgs e )	
		{
			Focus();
			var mouse_position = Mouse.GetPosition(m_gradient_box);

			foreach (var stop in m_gradient.GradientStops)
			{
				if (	mouse_position.X >= stop.Offset * m_gradient_box.ActualWidth - c_key_half_size
					&&	mouse_position.X <= stop.Offset * m_gradient_box.ActualWidth + c_key_half_size)
				{
					m_gradient_box.selected_gradient_stop = stop;
					m_gradient_box.InvalidateVisual( );
					if ( e.ChangedButton == MouseButton.Left )
					{
						m_last_mouse_position = Mouse.GetPosition( m_gradient_box );
						m_gradient_box.selected_gradient_stop = stop;
						
					}
					e.Handled = true;
					return;
				}
			}

			m_gradient_box.selected_gradient_stop = null;
			m_gradient_box.InvalidateVisual();
		}
		private void			gradient_box_mouse_move					( Object sender, MouseEventArgs e )			
		{
			var mouse_point		= Mouse.GetPosition( m_gradient_box );

			if ( m_stop_cached && e.LeftButton == MouseButtonState.Released )
			{
				m_stop_cached = false;
				m_gradient_box.ReleaseMouseCapture( );
				on_key_modification_done( m_colors_dict[m_gradient_box.selected_gradient_stop], m_gradient_box.selected_gradient_stop );
				return;
			}

			if ( e.LeftButton == MouseButtonState.Pressed && mouse_point != m_last_mouse_position )
			{
				if ( m_gradient_box.selected_gradient_stop == null )
					return;

				if ( !m_stop_cached ){
					m_stop_cached = true;
					m_gradient_box.CaptureMouse( );
					on_key_modification_start( m_colors_dict[m_gradient_box.selected_gradient_stop], m_gradient_box.selected_gradient_stop );
				}
					
				m_last_mouse_position = mouse_point;
				var new_position	= mouse_point.X;

				if( new_position < 0 )
					new_position		= 0;

				if( new_position > m_gradient_box.ActualWidth )
					new_position		= m_gradient_box.ActualWidth;

				m_gradient_box.selected_gradient_stop.Offset = new_position / m_gradient_box.ActualWidth;
				on_key_modified			( m_colors_dict[m_gradient_box.selected_gradient_stop], m_gradient_box.selected_gradient_stop );
				m_gradient_box.InvalidateVisual( );
			}
		}
		private void			gradient_box_mouse_up					( Object sender, MouseButtonEventArgs e )	
		{
			if ( m_stop_cached && e.ChangedButton == MouseButton.Left)
			{
				m_stop_cached = false;
				m_gradient_box.ReleaseMouseCapture( );
				on_key_modification_done( m_colors_dict[m_gradient_box.selected_gradient_stop], m_gradient_box.selected_gradient_stop );
				return;
			}
			if ( e.ChangedButton == MouseButton.Middle )
			{
				var list = ( (LinearGradientBrush)m_gradient_box.Background ).GradientStops;
				if ( list.Count > 1 && m_gradient_box.selected_gradient_stop != null )
				{
					on_key_removed	( m_colors_dict[m_gradient_box.selected_gradient_stop], m_gradient_box.selected_gradient_stop );
					list.Remove		( m_gradient_box.selected_gradient_stop );
					m_gradient_box.InvalidateVisual( );
				}
			}
		}
		private void			gradient_box_mouse_right_button_up		( Object sender, MouseButtonEventArgs e )	
		{
			var mouse_position = Mouse.GetPosition(m_gradient_box);

			if (m_gradient_box.selected_gradient_stop != null)
			{
				var list = ((LinearGradientBrush)m_gradient_box.Background).GradientStops;
				if (list.Count > 1)
					m_remove_context_menu.IsOpen = true;
			}
			else
			{
				var list = ((LinearGradientBrush)m_gradient_box.Background).GradientStops;
				m_add_context_menu.IsOpen = true;
			}
		}
		private void			content_control_mouse_double_click		( Object sender, MouseButtonEventArgs e )	
		{
			var stops					= m_gradient.GradientStops;
			var mouse_position			= Mouse.GetPosition(m_gradient_box);
			GradientStop closest_left	= stops[0];
			GradientStop closest_right	= stops[0];
			Double delta_left			= Double.MinValue;
			Double delta_right			= Double.MaxValue;

			foreach (var stop in m_gradient.GradientStops)
			{
				var stop_position = stop.Offset * m_gradient_box.ActualWidth;

				var mouse_to_stop_delta = stop_position - mouse_position.X;
				if (mouse_to_stop_delta >= 0)
				{
					if (mouse_to_stop_delta < delta_right)
					{
						delta_right = mouse_to_stop_delta;
						closest_right = stop;
					}
				}
				else
				{
					if (mouse_to_stop_delta > delta_left)
					{
						delta_left = mouse_to_stop_delta;
						closest_left = stop;
					}
				}
				if (	mouse_position.X >= stop_position - c_key_half_size
					&&	mouse_position.X <= stop_position + c_key_half_size)
				{
					m_gradient_box.selected_gradient_stop = stop;
					
					//Run Color Picker
					var dialog = new color_picker.dialog { selected_color = m_colors_dict[stop] };

					if ( dialog.ShowDialog( ) == true )
					{
						var dlg_color = dialog.selected_color;
						m_colors_dict[stop] = dlg_color;
						stop.Color = (Color)dialog.selected_color;
					}

					on_key_modified					( m_colors_dict[stop], stop );
					on_key_modification_done		( m_colors_dict[stop], stop );
					m_gradient_box.InvalidateVisual	( );
					return;
				}
			}

			Color color;

			if (delta_left == Double.MinValue)
				color = closest_right.Color; 
			else if (delta_right == Double.MaxValue)
				color = closest_left.Color;
			else
			{
				//Create new key
				var width = delta_right - delta_left;
				var right_coef = Math.Abs(delta_left) / width;
				var left_coef = Math.Abs(delta_right) / width;
				color = Color.FromArgb(
					(byte)(closest_left.Color.A * left_coef + closest_right.Color.A * right_coef),
					(byte)(closest_left.Color.R * left_coef + closest_right.Color.R * right_coef),
					(byte)(closest_left.Color.G * left_coef + closest_right.Color.G * right_coef),
					(byte)(closest_left.Color.B * left_coef + closest_right.Color.B * right_coef));

			}
			var new_stop					= new GradientStop(color, mouse_position.X/m_gradient_box.ActualWidth);
			m_gradient.GradientStops.Add	(new_stop);
			m_gradient_box.selected_gradient_stop = new_stop;

			m_colors_dict.Add				( new_stop, (color_rgb)new_stop.Color );
			on_key_added					( m_colors_dict[new_stop], new_stop );
			m_gradient_box.Focus			( );
			m_gradient_box.InvalidateVisual	( );
		}
		private void			remove_item_click						( Object sender, RoutedEventArgs e )		
		{
			var list = ((LinearGradientBrush)m_gradient_box.Background).GradientStops;
			list.Remove(m_gradient_box.selected_gradient_stop);
			on_key_removed( m_colors_dict[m_gradient_box.selected_gradient_stop], m_gradient_box.selected_gradient_stop );
			m_colors_dict.Remove( m_gradient_box.selected_gradient_stop );
			m_gradient_box.InvalidateVisual();
		}
		private void			add_item_click							( Object sender, RoutedEventArgs e ) 		
		{
			add_key_at_mouse_position();
		}
		
		private	void			add_key_at_mouse_position				( )											
		{
			var mouse_position			= Mouse.GetPosition(m_gradient_box);

			Color color = Colors.White;

			if (m_gradient.GradientStops.Count > 0)
			{
				GradientStop closest_left	= m_gradient.GradientStops[0];
				GradientStop closest_right	= m_gradient.GradientStops[0];
				Double delta_left			= Double.MinValue;
				Double delta_right			= Double.MaxValue;

				foreach (var stop in m_gradient.GradientStops)
				{
					var stop_position = stop.Offset * m_gradient_box.ActualWidth;

					var mouse_to_stop_delta = stop_position - mouse_position.X;
					if (mouse_to_stop_delta >= 0)
					{
						if (mouse_to_stop_delta < delta_right)
						{
							delta_right = mouse_to_stop_delta;
							closest_right = stop;
						}
					}
					else
					{
						if (mouse_to_stop_delta > delta_left)
						{
							delta_left = mouse_to_stop_delta;
							closest_left = stop;
						}
					}
				}
			
				if (delta_left == Double.MinValue)
					color = closest_right.Color; 
				else if (delta_right == Double.MaxValue)
					color = closest_left.Color;
				else
				{
					//Create new key
					var width = delta_right - delta_left;
					var right_coef = Math.Abs(delta_left) / width;
					var left_coef = Math.Abs(delta_right) / width;
					color = Color.FromArgb(
						(byte)(closest_left.Color.A * left_coef + closest_right.Color.A * right_coef),
						(byte)(closest_left.Color.R * left_coef + closest_right.Color.R * right_coef),
						(byte)(closest_left.Color.G * left_coef + closest_right.Color.G * right_coef),
						(byte)(closest_left.Color.B * left_coef + closest_right.Color.B * right_coef));

				}
			}
			var new_stop = new GradientStop(color, mouse_position.X/m_gradient_box.ActualWidth);
			m_gradient.GradientStops.Add( new_stop );
			m_colors_dict.Add( new_stop, (color_rgb)new_stop.Color );
			m_gradient_box.selected_gradient_stop = new_stop;
			on_key_added( m_colors_dict[new_stop], new_stop );
			m_gradient_box.Focus();
			m_gradient_box.InvalidateVisual();
		}

		protected void			on_key_added							( color_rgb color, GradientStop stop )		
		{
			if( key_added != null )
				key_added( this, new key_event_args( color, stop ) );
			
		}
		protected void			on_key_removed							( color_rgb color, GradientStop stop )		
		{
			if( key_removed != null )
				key_removed( this, new key_event_args( color, stop ) );
			
		}
		protected void			on_key_modified							( color_rgb color, GradientStop stop )		
		{
			if( key_modified != null )
				key_modified( this, new key_event_args( color, stop ) );
			
		}
		protected void			on_key_modification_start				( color_rgb color, GradientStop stop )		
		{
			if( key_modification_start != null )
				key_modification_start( this, new key_event_args( color, stop ) );
			
		}
		protected void			on_key_modification_done				( color_rgb color, GradientStop stop )		
		{
			if( key_modification_done != null ) 
				key_modification_done( this, new key_event_args( color, stop ) );
			
		}

		internal void			invalidate								( )											
		{
			m_gradient_box.InvalidateVisual();
		}

		internal GradientStop	add_key									( Double offset, color_rgb color )			
		{
			var ret_val				= new GradientStop( (Color)color, offset );
			var linear_gradient		= (LinearGradientBrush)m_gradient_box.Background;
			linear_gradient.GradientStops.Add( ret_val );
			m_colors_dict.Add		( ret_val, color );

			return ret_val;
		}
		internal void			clear									( )											
		{
			( (LinearGradientBrush)m_gradient_box.Background ).GradientStops.Clear( );
			m_colors_dict.Clear( );
		}

		
		#endregion

		#region | InnerTypes |


		public class	key_event_args:EventArgs	
		{
			public key_event_args (color_rgb set_color, GradientStop set_stop)
			{
				color	= set_color;
				stop	= set_stop;
			}

			public color_rgb		color;
			public GradientStop		stop;
		}


		#endregion

	}

	internal class custom_gradient_control : Border	
	{
		public GradientStop selected_gradient_stop;

		public Int32		selected_index				
		{
			get
			{
				var gradient = (LinearGradientBrush)Background;
				return gradient.GradientStops.IndexOf( selected_gradient_stop );
			}
		}

		protected override		void		OnRender	( DrawingContext dc )	
		{
			base.OnRender( dc );

			var gradient = (LinearGradientBrush)Background;

			foreach (var gradient_stop in gradient.GradientStops)
			{
				var new_color		= Colors.White - gradient_stop.Color;
				new_color.A			= 255;
				var rect			= new Rect( ActualWidth * gradient_stop.Offset - color_ramp.c_key_half_size, 0, color_ramp.c_key_half_size * 2, ActualHeight );
				dc.DrawRectangle	( new SolidColorBrush( gradient_stop.Color ), new Pen( new SolidColorBrush( new_color ), 1 ), rect );
			}
		}
	}
}
