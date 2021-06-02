////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Input;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public partial class animation_lexeme_panel: INotifyPropertyChanged
	{

		#region | Initialize |


		public	animation_lexeme_panel	( )	
		{
			InitializeComponent();

			m_scale_multiplier = 1.1f;
			m_max_time = 0.0f;
			time_layout_ruler.top_to_down = true;
			time_layout_ruler.layout_scale = ( ) => time_layout_scale;
			time_layout_ruler.layout_width = ( ) => max_time_scaled;
			time_layout_ruler.layout_offset	= ( ) => 0.0f;
			
			m_animations = new ObservableCollection<animation_item>();
			m_animation_names.ItemsSource = m_animations;
	        m_animation_intervals_view.ItemsSource = m_animations;
			m_animations.CollectionChanged += delegate
			{
				recalc_max_time();
			};

			m_lexeme = new animation_lexeme_item();
			m_lexeme.panel = this;
			ObservableCollection<animation_lexeme_item> lst = new ObservableCollection<animation_lexeme_item>();
			lst.Add(m_lexeme);
			m_lexeme_view.ItemsSource = lst;
			m_lexeme.intervals.CollectionChanged += delegate
			{
				recalc_max_time();
			};
		}


		#endregion

		#region |   Events   |


		public event PropertyChangedEventHandler			PropertyChanged;
		public event EventHandler							lexeme_modified;

	
		#endregion

		#region |   Fields   |


		private		Single									m_time_layout_scale = 1.0f;
		private		Single									m_scale_multiplier;
		private		Single									m_max_time;
		private		Single									m_last_mouse_down_position;
		private		Boolean									m_pan_mode;
		private		ObservableCollection<animation_item>	m_animations;
		private		animation_lexeme_item					m_lexeme;


		#endregion

		#region | Properties |


		public	animation_lexeme_item					lexeme
		{
			get
			{
				return m_lexeme;
			}
		}
		public	ObservableCollection<animation_item> animations
		{
		    get
		    {
		        return m_animations;
		    }
		}
		public	Single		time_layout_scale				
		{
			get 
			{ 
				return m_time_layout_scale; 
			}
			set 
			{ 
				m_time_layout_scale = value;
				on_property_changed("time_layout_scale");
				on_property_changed("max_time_scaled");
				time_layout_ruler.InvalidateVisual();
				if(m_animations!=null)
				{
				    foreach(var item in m_animations)
				        item.update();
				}

				if(m_lexeme!=null)
					m_lexeme.update();
			}
		}
		public	Single		max_time						
		{
			get 
			{ 
				return m_max_time; 
			}
		}
		public	Single		max_time_scaled					
		{
			get 
			{
				return m_max_time * m_time_layout_scale;
			}
		}


		#endregion

		#region |   Methods  |

		private		void	recalc_max_time					()												
		{
			Single max_animation_length = 0.0f;
			foreach(var item in m_animations)
			{
				if(item.length > max_animation_length)
					max_animation_length = item.length;
			}

			if(max_animation_length < m_lexeme.intervals_length)
				max_animation_length = m_lexeme.intervals_length; 

			m_max_time = max_animation_length / m_time_layout_scale;
			Single window_width = (Single)m_intervals_h_scroller.ActualWidth;
			if(window_width>0 && m_max_time>0.0f)
				time_layout_scale = window_width/m_max_time;

			on_property_changed("max_time");
			on_property_changed("max_time_scaled");
			if(lexeme_modified!=null)
				lexeme_modified(this, EventArgs.Empty);
		}
		internal void on_lexeme_modified()
		{
			if(lexeme_modified!=null)
				lexeme_modified(this, EventArgs.Empty);
		}
		private		void	on_property_changed				( String property_name )						
		{
			if( PropertyChanged != null )
				PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
		}
		private		void	user_control_loaded				( Object sender, RoutedEventArgs e )			
		{
			var vertical_scroll = (ScrollBar)m_intervals_scroller.Template.FindName("PART_VerticalScrollBar", m_intervals_scroller);
			vertical_scroll.Scroll += m_vertical_scrollbar_scroll;

			m_vertical_scrollbar.SetBinding( ScrollBar.ViewportSizeProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(ScrollBar.ViewportSizeProperty), Mode = BindingMode.OneWay,UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_vertical_scrollbar.SetBinding( RangeBase.ValueProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.ValueProperty), Mode = BindingMode.OneWay, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_vertical_scrollbar.SetBinding( RangeBase.MinimumProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.MinimumProperty), Mode = BindingMode.OneWay} );
			m_vertical_scrollbar.SetBinding( RangeBase.MaximumProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.MaximumProperty), Mode = BindingMode.OneWay} );

			var names_scrollbar = (ScrollBar)m_names_scroller.Template.FindName("PART_VerticalScrollBar", m_names_scroller);
			names_scrollbar.Scroll += m_vertical_scrollbar_scroll;
			
			var horizontal_scroll = (ScrollBar)m_intervals_h_scroller.Template.FindName("PART_HorizontalScrollBar", m_intervals_h_scroller);
			m_horizontal_scrollbar.SetBinding( ScrollBar.ViewportSizeProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(ScrollBar.ViewportSizeProperty), Mode = BindingMode.OneWay,UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_horizontal_scrollbar.SetBinding( RangeBase.ValueProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.ValueProperty), Mode = BindingMode.OneWay, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_horizontal_scrollbar.SetBinding( RangeBase.MinimumProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.MinimumProperty), Mode = BindingMode.OneWay} );
			m_horizontal_scrollbar.SetBinding( RangeBase.MaximumProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.MaximumProperty), Mode = BindingMode.OneWay} );
			if(m_max_time>0.0f)
				time_layout_scale = (Single)m_intervals_h_scroller.ActualWidth/m_max_time;

			time_layout_ruler.InvalidateVisual();
		}
		private		void	m_vertical_scrollbar_scroll		( Object sender, ScrollEventArgs e )			
		{
			m_names_scroller.ScrollToVerticalOffset( e.NewValue );
			m_intervals_scroller.ScrollToVerticalOffset( e.NewValue );
			m_vertical_scrollbar.Value = e.NewValue;
		}
		private		void	scroll_viewer_scroll_changed	( Object sender, ScrollChangedEventArgs e )		
		{
			m_names_scroller.ScrollToVerticalOffset( e.VerticalOffset );
			m_intervals_scroller.ScrollToVerticalOffset( e.VerticalOffset );
			m_vertical_scrollbar.Value = e.VerticalOffset;
		}
		private		void	m_horizontal_scrollbar_scroll	( Object sender, ScrollEventArgs e )			
		{
			m_intervals_h_scroller.ScrollToHorizontalOffset( e.NewValue );
		}
		private		void	time_layout_control_mouse_wheel	( Object sender, MouseWheelEventArgs e )		
		{
			e.Handled = true;			
			if ( Keyboard.IsKeyDown( Key.LeftAlt ) )
			{
				if (e.Delta > 0)
				{
					if ( time_layout_scale < 15 )
						time_layout_scale *= m_scale_multiplier;
				}
				else
				{
					if ( time_layout_scale > 0.1 )
						time_layout_scale /= m_scale_multiplier;

					Single window_width = (Single)m_intervals_h_scroller.ActualWidth;
					if(window_width>0 && m_max_time>0.0f && time_layout_scale<window_width/m_max_time)
						time_layout_scale = window_width/m_max_time;
				}
			}
			else if ( Keyboard.IsKeyDown( Key.LeftCtrl ) )
			{
				if (e.Delta < 0)
					m_intervals_scroller.LineDown( );
				else
					m_intervals_scroller.LineUp( );
			}
			else
			{
				if (e.Delta < 0)
					ScrollBar.LineLeftCommand.Execute( null, m_horizontal_scrollbar );
				else
					ScrollBar.LineRightCommand.Execute( null, m_horizontal_scrollbar );
			}
		}
		private		void	own_grid_mouse_move				( Object sender, MouseEventArgs e )				
		{
			float offset = (float)e.GetPosition(this).X - m_last_mouse_down_position;
			if(m_pan_mode)
				m_intervals_h_scroller.ScrollToHorizontalOffset(m_intervals_h_scroller.HorizontalOffset - offset);

			m_last_mouse_down_position = (float)e.GetPosition(this).X;
		}
		private		void	own_grid_mouse_left_button_up	( Object sender, MouseButtonEventArgs e )		
		{
			own_grid.ReleaseMouseCapture();
			m_pan_mode = false;
		}
		private		void	own_grid_mouse_left_button_down	( Object sender, MouseButtonEventArgs e	)		
		{
			if(!Keyboard.IsKeyDown(Key.LeftAlt)) 
				return;

			e.Handled = true;
			m_pan_mode = true;
			own_grid.CaptureMouse();
		}
		public		void	remove_animation				( animation_item item )			
		{
			foreach(var interval in item.intervals)
				m_lexeme.remove_all_interval_views(interval);

			m_animations.Remove(item);
		}
		private		void	user_control_drag_over			(Object o, DragEventArgs e)
		{
			if(e.Effects==DragDropEffects.Link)
			{
				e.Effects = DragDropEffects.None;
				e.Handled = true;
			}
		}
		private		void	user_control_drop				(Object o, DragEventArgs e)
		{
			if(e.Effects==DragDropEffects.Move)
			{
				Object data = e.Data.GetData(typeof(int));
				lexeme.remove_interval((int)data);
			}
		}
		#endregion

	}
}
