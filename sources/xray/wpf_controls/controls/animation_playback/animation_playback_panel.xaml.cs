////////////////////////////////////////////////////////////////////////////
//	Created		: 02.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Input;

namespace xray.editor.wpf_controls.animation_playback
{
	/// <summary>
	/// Interaction logic for animation_playback_control.xaml
	/// </summary>
	public partial class animation_playback_panel: INotifyPropertyChanged
	{

		#region | Initialize |


		public	animation_playback_panel	( )	
		{
			InitializeComponent();

			animation_time			= 0;
			m_scale_multiplier		= 1.1f;
			follow_animation_thumb	= true;
			snap_to_frames			= false;
			m_fps					= 30;
			editor_is_paused		= true;
            DataContextChanged		+= ( o, e )	=> on_property_changed( "animation_thumb_position" );
            DataContext				= this;
			
			time_layout_ruler_s.top_to_down		= false;
			time_layout_ruler_s.layout_scale	= ( ) => time_layout_scale;
			time_layout_ruler_s.layout_width	= ( ) => max_time_scaled;
			time_layout_ruler_s.layout_offset	= ( ) => 0.0f;

			time_layout_ruler_f.top_to_down		= true;
			time_layout_ruler_f.layout_scale	= ( ) => time_layout_scale*1000.0f / fps;
			time_layout_ruler_f.layout_width	= ( ) => max_time_scaled;
			time_layout_ruler_f.layout_offset	= ( ) => 0.0f;
		}


		#endregion

		#region |   Events   |


		public event PropertyChangedEventHandler PropertyChanged;
		

		#endregion

		#region |   Fields   |


		private		Single		m_time_layout_scale = 1.0f;
		private		Single		m_scale_multiplier;
		private		Single		m_max_time;
		private		Single		m_last_mouse_down_position;
		private		Boolean		m_pan_mode;
		private		Single		m_animation_time;
		private		Single		m_fps;

		public		Boolean		editor_is_paused;
		public		Single		fps
		{
			get { return m_fps; }
			set 
			{ 
				m_fps = value;
				on_property_changed("fps");
				on_property_changed("max_time_scaled");
				animation_time = animation_time;
				time_layout_ruler_s.InvalidateVisual();
				time_layout_ruler_f.InvalidateVisual();
				if(animation_items != null)
				{
					foreach (var item in animation_items)
						item.update();
				}
			}
		}


		#endregion

		#region | Properties |


		public	Single		time_layout_scale				
		{
			get { return m_time_layout_scale; }
			set 
			{ 
				m_time_layout_scale = value;
				on_property_changed("time_layout_scale");
				on_property_changed("max_time_scaled");
				animation_time = animation_time;
				time_layout_ruler_s.InvalidateVisual();
				time_layout_ruler_f.InvalidateVisual();
				if (animation_items != null)
				{
					foreach (var item in animation_items)
						item.update();
				}
			}
		}
		public	Single		max_time						
		{
			get { return m_max_time; }
			set 
			{
				m_max_time = value;
				if (m_max_time < m_animation_time)
					animation_time = m_max_time;

				Single window_width = (Single)m_animations_h_scroller.ActualWidth;
				if(window_width>0 && m_max_time>0.0f && time_layout_scale<window_width/m_max_time)
				    time_layout_scale = window_width/m_max_time;

				on_property_changed("max_time");
				on_property_changed("max_time_scaled");
			}
		}
		public	Single		max_time_scaled					
		{
			get { return m_max_time * m_time_layout_scale; }
			set	{ }
		}
		public	Boolean		follow_animation_thumb			
		{
			get;
			set;
		}
		public	Boolean		snap_to_frames					
		{
			get;
			set;
		}
		public	String		current_time_text_frames		
		{
			get
			{
				float val = (float)System.Math.Round(m_animation_time / 1000.0f * fps, 3);
				return val.ToString() + " frames";
			}
			set
			{ }
		}
		public	String		current_time_text_seconds		
		{
			get
			{
				float val = (float)System.Math.Round(m_animation_time / 1000.0f, 3);
				return val.ToString() + " seconds";
			}
			set
			{ }
		}
		public	IEnumerable<animation_item> animation_items	
		{
			get
			{
				return (IEnumerable<animation_item>)m_animation_item_views.ItemsSource;
			}
			set
			{
				if (value != null)
				{
					foreach (var item in value)
						item.m_panel = this;
				}

				m_animation_item_views.ItemsSource = value;
				m_animation_item_names.ItemsSource = value;
			}
		}
		public	Single		animation_time					
		{
			get 
			{
				return m_animation_time; 
			}
			set 
			{
				m_animation_time = value;
				if( snap_to_frames && editor_is_paused )
					m_animation_time = (Single)Math.Round( value * fps / 1000.0f ) * 1000.0f / fps;

				if( m_animation_time > m_max_time )
					m_animation_time = m_max_time;

				on_property_changed( "animation_time" );
                on_property_changed( "animation_thumb_position" );
				on_property_changed( "current_time_text_frames" );
				on_property_changed( "current_time_text_seconds" );
				if ( animation_items != null )
					foreach ( var item in animation_items )
						item.bindings.update_bindings( );

				if( follow_animation_thumb )
				{
					var scroll_to = value * m_time_layout_scale - ( m_animations_h_scroller.ViewportWidth / 2);

					if( scroll_to < 0.0f )
						scroll_to = 0.0f;

					if( scroll_to > max_time * m_time_layout_scale )
						scroll_to = max_time * m_time_layout_scale;

					m_animations_h_scroller.ScrollToHorizontalOffset( scroll_to );
				}
			}
		}
        public  Single		animation_thumb_position		
        {
            get
            {
				return (Single)(animation_time * m_time_layout_scale - m_time_thumb.Width / 2);
            }
            set
            {}
		}


		#endregion

		#region |   Methods  |


		private		void	thumb_drag_delta				( Object sender, DragDeltaEventArgs e )		
		{
			var thumb_half_width = 2;
			var change = ( e.HorizontalChange > 100.0f ) ? e.HorizontalChange / 100.0f : e.HorizontalChange;
			var new_position = Math.Max( 0 , Math.Min( animation_time + change, own_grid.ActualWidth/m_time_layout_scale - thumb_half_width - 1 ) );
			animation_time = (Single)new_position;
		}
		private		void	on_property_changed				( String property_name )					
		{
			if( PropertyChanged != null )
				PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
		}
		private		void	user_control_loaded				( Object sender, RoutedEventArgs e )		
		{
			var vertical_scroll = (ScrollBar)m_animations_scroller.Template.FindName("PART_VerticalScrollBar", m_animations_scroller);
			vertical_scroll.Scroll += m_vertical_scrollbar_scroll;

			m_vertical_scrollbar.SetBinding( ScrollBar.ViewportSizeProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(ScrollBar.ViewportSizeProperty), Mode = BindingMode.OneWay,UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_vertical_scrollbar.SetBinding( RangeBase.ValueProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.ValueProperty), Mode = BindingMode.OneWay, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_vertical_scrollbar.SetBinding( RangeBase.MinimumProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.MinimumProperty), Mode = BindingMode.OneWay} );
			m_vertical_scrollbar.SetBinding( RangeBase.MaximumProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.MaximumProperty), Mode = BindingMode.OneWay} );

			var names_scrollbar = (ScrollBar)m_names_scroller.Template.FindName("PART_VerticalScrollBar", m_names_scroller);
			names_scrollbar.Scroll += m_vertical_scrollbar_scroll;
			
			var horizontal_scroll = (ScrollBar)m_animations_h_scroller.Template.FindName("PART_HorizontalScrollBar", m_animations_h_scroller);
			m_horizontal_scrollbar.SetBinding( ScrollBar.ViewportSizeProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(ScrollBar.ViewportSizeProperty), Mode = BindingMode.OneWay,UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_horizontal_scrollbar.SetBinding( RangeBase.ValueProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.ValueProperty), Mode = BindingMode.OneWay, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_horizontal_scrollbar.SetBinding( RangeBase.MinimumProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.MinimumProperty), Mode = BindingMode.OneWay} );
			m_horizontal_scrollbar.SetBinding( RangeBase.MaximumProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.MaximumProperty), Mode = BindingMode.OneWay} );
			if(m_max_time>0.0f)
			    time_layout_scale = (Single)m_animations_h_scroller.ActualWidth/m_max_time;

			time_layout_ruler_s.InvalidateVisual();
			time_layout_ruler_f.InvalidateVisual();
		}
		private		void	m_vertical_scrollbar_scroll		( Object sender, ScrollEventArgs e )		
		{
			m_names_scroller.ScrollToVerticalOffset( e.NewValue );
			m_animations_scroller.ScrollToVerticalOffset( e.NewValue );
			m_vertical_scrollbar.Value = e.NewValue;
		}
		private		void	scroll_viewer_scroll_changed	( Object sender, ScrollChangedEventArgs e )	
		{
			m_names_scroller.ScrollToVerticalOffset( e.VerticalOffset );
			m_animations_scroller.ScrollToVerticalOffset( e.VerticalOffset );
			m_vertical_scrollbar.Value = e.VerticalOffset;
		}
		private		void	m_horizontal_scrollbar_scroll	( Object sender, ScrollEventArgs e )		
		{
			m_animations_h_scroller.ScrollToHorizontalOffset( e.NewValue );
		}
        private     void    timeline_mouse_down             ( Object sender, MouseButtonEventArgs e )	
        {
			Single v = (Single)e.GetPosition(m_timeline_up).X / m_time_layout_scale;
			if(snap_to_frames && editor_is_paused)
				v = (Single)System.Math.Round(v * fps / 1000.0f) * 1000.0f / fps;

			animation_time = v;
        }
		private		void	time_layout_control_mouse_wheel	( Object sender, MouseWheelEventArgs e )	
		{
			e.Handled = true;			
			if ( Keyboard.IsKeyDown( Key.LeftAlt ) )
			{
				if (e.Delta > 0)
				{
					if ( time_layout_scale < 25 )
						time_layout_scale *= m_scale_multiplier;
				}
				else
				{
					if ( time_layout_scale > 0.04 )
						time_layout_scale /= m_scale_multiplier;

					Single window_width = (Single)m_animations_h_scroller.ActualWidth;
					if(window_width>0 && m_max_time>0.0f && time_layout_scale<window_width/m_max_time)
					    time_layout_scale = window_width/m_max_time;
				}
			}
			else if ( Keyboard.IsKeyDown( Key.LeftCtrl ) )
			{
				if (e.Delta < 0)
					m_animations_scroller.LineDown( );
				else
					m_animations_scroller.LineUp( );
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
				m_animations_h_scroller.ScrollToHorizontalOffset(m_animations_h_scroller.HorizontalOffset - offset);

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


		#endregion

	}
}
