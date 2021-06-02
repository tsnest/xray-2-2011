////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
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

namespace xray.editor.wpf_controls.animation_setup
{
	/// <summary>
	/// Interaction logic for animation_setup_panel.xaml
	/// </summary>
	public partial class animation_setup_panel: INotifyPropertyChanged
	{

		#region | Initialize |


		public	animation_setup_panel	( )	
		{
			InitializeComponent();

			channels				= new ObservableCollection<animation_channel>();
			m_context_menu			= (ContextMenu)FindResource("panel_context_menu");
			animation_time			= 0;
			m_animation_length		= 0;
			m_scale_multiplier		= 1.1f;
			follow_animation_thumb	= true;
			snap_to_frames			= true;
			m_fps					= 30;
			editor_is_paused		= true;
			DataContextChanged		+= ( o, e )	=> on_property_changed( "animation_thumb_position" );
			DataContext				= this;
			
			time_layout_ruler_s.top_to_down		= true;
			time_layout_ruler_s.layout_scale	= ( ) => time_layout_scale;
			time_layout_ruler_s.layout_width	= ( ) => max_time_scaled;
			time_layout_ruler_s.layout_offset	= ( ) => 0.0f;

			time_layout_ruler_f.top_to_down		= false;
			time_layout_ruler_f.layout_scale	= ( ) => time_layout_scale*1000.0f / fps;
			time_layout_ruler_f.layout_width	= ( ) => max_time_scaled;
			time_layout_ruler_f.layout_offset	= ( ) => 0.0f;
		}


		#endregion

		#region |   Events   |


		public event PropertyChangedEventHandler				PropertyChanged;
		public event EventHandler<setup_panel_event_args>		ask_for_create_channel;
		public event EventHandler<setup_panel_event_args>		ask_for_remove_channel;
		public event EventHandler<setup_panel_event_args>		ask_for_create_item;
		public event EventHandler<setup_panel_event_args>		ask_for_remove_item;
		public event EventHandler<setup_panel_event_args>		ask_for_change_item_property;
		public event EventHandler<setup_panel_copy_event_args>	ask_for_copy_channel;
		public event EventHandler<setup_panel_copy_event_args>	ask_for_cut_channel;
		public event EventHandler								ask_for_paste_channel;

	
		#endregion

		#region |   Fields   |


		private		Single									m_time_layout_scale = 1.0f;
		private		Single									m_scale_multiplier;
		private		Single									m_max_time;
		private		Single									m_last_mouse_down_position;
		private		Boolean									m_pan_mode;
		private		Single									m_animation_time;
		private		Single									m_fps;
		private		Single									m_animation_length;
		public		Boolean									editor_is_paused;
		private		readonly ContextMenu					m_context_menu;
		private		ObservableCollection<animation_channel> m_channels;


		#endregion

		#region | Properties |


		public	ObservableCollection<animation_channel> channels
		{
		    get
		    {
		        return m_channels;
		    }
		    set
		    {
				m_channels = value;
				if(value!=null)
				{
					foreach(var item in value)
				        item.panel = this;
				}

				m_animation_channel_names.ItemsSource = m_channels;
		        m_animation_channel_views.ItemsSource = m_channels;
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
				animation_time = animation_time;
				time_layout_ruler_s.InvalidateVisual();
				time_layout_ruler_f.InvalidateVisual();
				if(m_channels!=null)
				{
				    foreach(var item in m_channels)
				        item.update();
				}
			}
		}
		public	Single		max_time						
		{
			get 
			{ 
				return m_max_time; 
			}
			set 
			{
				m_max_time = value;
				if(m_max_time < m_animation_time)
					animation_time = m_max_time;

				if(m_channels!=null)
				{
				    foreach (var ch in m_channels)
				        ch.update_list_view();
				}

				Single window_width = (Single)m_channels_h_scroller.ActualWidth - 20.0f;
				if(window_width>0 && m_max_time>0.0f && time_layout_scale<window_width/m_max_time)
				    time_layout_scale = window_width/m_max_time;

				on_property_changed("max_time");
				on_property_changed("max_time_scaled");
			}
		}
		public	Single		max_time_scaled					
		{
			get 
			{ 
				return m_max_time * m_time_layout_scale; 
			}
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
		public	Single		animation_time					
		{
			get 
			{
				return m_animation_time; 
			}
			set 
			{
				m_animation_time = value;
				if(m_animation_time<0)
					m_animation_time = 0;

				if( snap_to_frames && editor_is_paused )
					m_animation_time = (Single)Math.Round( value * fps / 1000.0f ) * 1000.0f / fps;

				if(m_animation_time>m_max_time)
					m_animation_time = m_max_time;

				on_property_changed( "animation_time" );
                on_property_changed( "animation_thumb_position" );
				on_property_changed( "current_time_text_frames" );
				on_property_changed( "current_time_text_seconds" );

				if( follow_animation_thumb )
				{
					var scroll_to = value * m_time_layout_scale - ( m_channels_h_scroller.ViewportWidth / 2);

					if( scroll_to < 0.0f )
						scroll_to = 0.0f;

					if( scroll_to > max_time * m_time_layout_scale )
						scroll_to = max_time * m_time_layout_scale;

					m_channels_h_scroller.ScrollToHorizontalOffset( scroll_to );
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
		public	Single		fps								
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
				if(channels != null)
				{
					foreach (var item in channels)
						item.update();
				}
			}
		}
		public	Single		animation_length				
		{
			get
			{
				return m_animation_length;
			}
			set
			{
				m_animation_length = value;
				Single window_width = (Single)m_channels_h_scroller.ActualWidth - 20.0f;
				if(window_width>0 && m_max_time>0.0f)
				    time_layout_scale = window_width/m_max_time;
			}
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
			var vertical_scroll = (ScrollBar)m_channels_scroller.Template.FindName("PART_VerticalScrollBar", m_channels_scroller);
			vertical_scroll.Scroll += m_vertical_scrollbar_scroll;

			m_vertical_scrollbar.SetBinding( ScrollBar.ViewportSizeProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(ScrollBar.ViewportSizeProperty), Mode = BindingMode.OneWay,UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_vertical_scrollbar.SetBinding( RangeBase.ValueProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.ValueProperty), Mode = BindingMode.OneWay, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_vertical_scrollbar.SetBinding( RangeBase.MinimumProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.MinimumProperty), Mode = BindingMode.OneWay} );
			m_vertical_scrollbar.SetBinding( RangeBase.MaximumProperty, new Binding{Source = vertical_scroll,Path = new PropertyPath(RangeBase.MaximumProperty), Mode = BindingMode.OneWay} );

			var names_scrollbar = (ScrollBar)m_names_scroller.Template.FindName("PART_VerticalScrollBar", m_names_scroller);
			names_scrollbar.Scroll += m_vertical_scrollbar_scroll;
			
			var horizontal_scroll = (ScrollBar)m_channels_h_scroller.Template.FindName("PART_HorizontalScrollBar", m_channels_h_scroller);
			m_horizontal_scrollbar.SetBinding( ScrollBar.ViewportSizeProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(ScrollBar.ViewportSizeProperty), Mode = BindingMode.OneWay,UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_horizontal_scrollbar.SetBinding( RangeBase.ValueProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.ValueProperty), Mode = BindingMode.OneWay, UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged} );
			m_horizontal_scrollbar.SetBinding( RangeBase.MinimumProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.MinimumProperty), Mode = BindingMode.OneWay} );
			m_horizontal_scrollbar.SetBinding( RangeBase.MaximumProperty, new Binding{Source = horizontal_scroll,Path = new PropertyPath(RangeBase.MaximumProperty), Mode = BindingMode.OneWay} );
			if(m_max_time>0.0f)
			    time_layout_scale = ((Single)m_channels_h_scroller.ActualWidth - 20.0f)/m_max_time;

			time_layout_ruler_s.InvalidateVisual();
			time_layout_ruler_f.InvalidateVisual();
		}
		private		void	m_vertical_scrollbar_scroll		( Object sender, ScrollEventArgs e )								
		{
			m_names_scroller.ScrollToVerticalOffset( e.NewValue );
			m_channels_scroller.ScrollToVerticalOffset( e.NewValue );
			m_vertical_scrollbar.Value = e.NewValue;
		}
		private		void	scroll_viewer_scroll_changed	( Object sender, ScrollChangedEventArgs e )							
		{
			m_names_scroller.ScrollToVerticalOffset( e.VerticalOffset );
			m_channels_scroller.ScrollToVerticalOffset( e.VerticalOffset );
			m_vertical_scrollbar.Value = e.VerticalOffset;
		}
		private		void	m_horizontal_scrollbar_scroll	( Object sender, ScrollEventArgs e )								
		{
			m_channels_h_scroller.ScrollToHorizontalOffset( e.NewValue );
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
					if ( time_layout_scale < 15 )
						time_layout_scale *= m_scale_multiplier;
				}
				else
				{
					if ( time_layout_scale > 0.1 )
						time_layout_scale /= m_scale_multiplier;

					//Single window_width = (Single)m_channels_h_scroller.ActualWidth;
					//if(window_width>0 && m_max_time>0.0f && time_layout_scale<window_width/m_max_time)
					//    time_layout_scale = window_width/m_max_time;
				}
			}
			else if ( Keyboard.IsKeyDown( Key.LeftCtrl ) )
			{
				if (e.Delta < 0)
					m_channels_scroller.LineDown( );
				else
					m_channels_scroller.LineUp( );
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
				m_channels_h_scroller.ScrollToHorizontalOffset(m_channels_h_scroller.HorizontalOffset - offset);

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
		private		void	channels_scroller_mouse_right_button_down(Object o, MouseButtonEventArgs e)							
		{
			m_context_menu.IsOpen = true;
		}
		private		void	add_new_channel_click			(Object o, RoutedEventArgs e)										
		{
			MenuItem item = (MenuItem)o;
			try_add_channel((String)item.Header);
		}
		private		void	paste_channel_click				(Object o, RoutedEventArgs e)										
		{
			if(ask_for_paste_channel!=null)
				ask_for_paste_channel(this, EventArgs.Empty);
			else
				paste_channel();
		}
		private		void	copy_channel					(String channel_name)												
		{
			//implement here default copying of channel;
		}
		private		void	cut_channel						(String channel_name)												
		{
			//implement here default cutting of channel;
		}
		private		void	paste_channel					()																	
		{
			//implement here default pasting of channel;
		}

		private	animation_channel		channel_by_name		(String channel_name)												
		{
			if(m_channels==null)
				return null;

			foreach(var ch in m_channels)
			{
				if(ch.name==channel_name)
					return ch;
			}

			return null;
		}
		private	animation_channel_item	item_by_id			(animation_channel channel, Guid item_id)							
		{
			if(channel==null || channel.objects==null)
				return null;

			foreach(var item in channel.objects)
			{
				if(item.id.Equals(item_id))
					return item;
			}

			return null;
		}

		internal	void	try_remove_channel		(String channel_name)														
		{
			if(ask_for_remove_channel==null)
				remove_channel(channel_name);
			else
				ask_for_remove_channel(this, new setup_panel_event_args(channel_name, null, null, null));
		}
		internal	void	try_add_channel			(String channel_name)														
		{
			if(ask_for_create_channel!=null)
				ask_for_create_channel(this, new setup_panel_event_args(channel_name, null, null, null));
			else
			{
				animation_channel_type t;
				if(channel_name=="anim_intervals" || channel_name=="foot_steps")
					t = animation_channel_type.partitions;
				else if(channel_name=="intervals")
					t = animation_channel_type.intervals;
				else if(channel_name=="sound_events")
					t = animation_channel_type.events;
				else
					return;

				add_channel(channel_name, t);
			}
		}
		internal	void	try_remove_item			(String channel_name, Guid item_id)											
		{
			if(ask_for_remove_item==null)
			    remove_item(channel_name, item_id);
			else
			    ask_for_remove_item(this, new setup_panel_event_args(channel_name, item_id.ToString(), null,					 null));
		}
		internal	void	try_add_item			(String channel_name, Single position, Guid partition_to_split_id)			
		{
			if(ask_for_create_item==null)
			    add_item(channel_name, position, partition_to_split_id);
			else
			    ask_for_create_item(this, new setup_panel_event_args(channel_name, partition_to_split_id.ToString(), null, position));
		}
		internal	void	try_change_item_property(String channel_name, Guid item_id, String property_name, Object new_val)	
		{
			if(ask_for_change_item_property==null)
				change_item_property(channel_name, item_id, property_name, new_val);
			else
			    ask_for_change_item_property(this, new setup_panel_event_args(channel_name, item_id.ToString(), property_name, new_val));
		}
		internal	void	try_change_item_properties(String channel_name, List<Guid> item_ids, List<String> property_names, List<Object> new_vals) 
		{
			if(ask_for_change_item_property==null)
			{
				for(int i=0; i<item_ids.Count; ++i)
					change_item_property(channel_name, item_ids[i], property_names[i], new_vals[i]);
			}
			else
			{
				List<String> item_ids_str = new List<String>();
				foreach(var item_id in item_ids)
					item_ids_str.Add(item_id.ToString());

			    ask_for_change_item_property(this, new setup_panel_event_args(channel_name, item_ids_str, property_names, new_vals));
			}
		}
		internal	void	try_copy_channel		(String channel_name)														
		{
			if(ask_for_change_item_property==null)
				copy_channel(channel_name);
			else
			    ask_for_copy_channel(this, new setup_panel_copy_event_args(channel_name));
		}
		internal	void	try_cut_channel			(String channel_name)														
		{
			if(ask_for_change_item_property==null)
				cut_channel(channel_name);
			else
			    ask_for_cut_channel(this, new setup_panel_copy_event_args(channel_name));
		}

		public		void	change_item_property	(String channel_name, Guid item_id, String property_name, Object new_val)	
		{
			animation_channel ch = channel_by_name(channel_name);
			if(ch==null)
				return;

			animation_channel_item item = item_by_id(ch, item_id);
			if(item==null)
				return;

			item.change_property(property_name, new_val);
		}
		public	animation_channel		add_channel	(String name, animation_channel_type t)										
		{
			animation_channel ch = channel_by_name(name);
			if(ch!=null)
				return ch;

			ch = new animation_channel(name, animation_length, t);
			ch.panel = this;
			m_channels.Add(ch);
			return ch;
		}
		public	animation_channel_item	add_item	(String channel_name, Single position, Guid partition_to_split_id)			
		{
			animation_channel ch = channel_by_name(channel_name);
			if(ch==null)
				return null;

			animation_channel_item item;
			int item_insert_index = 0;
			if(ch.type==animation_channel_type.intervals)
			{
				Single start_time = position/m_time_layout_scale + 1;
				Single length = snap_to_frames ? 1000/fps : 10;
				Single left_x = 0;
				Single right_x = ch.length/m_time_layout_scale;
				int my_index = 0;
				for(int index=0; index<ch.objects.Count; index++)
				{
					animation_channel_interval i = (animation_channel_interval)ch.objects[index];
					if((i.start_time+i.length)/m_time_layout_scale < start_time)
					{
						left_x = (i.start_time+i.length)/m_time_layout_scale;
						my_index = index + 1;
						if(index < ch.objects.Count-1)
							right_x = ((animation_channel_interval)ch.objects[index+1]).start_time/m_time_layout_scale;
						else
							right_x = ch.length/m_time_layout_scale;
					}
				}

				if(right_x-left_x-length<-0.0001)
					return null;

				if(start_time+length > right_x)
					start_time = right_x - length;

				item = new animation_channel_interval(ch, start_time, length, 0);
			}
			else if(ch.type==animation_channel_type.partitions)
			{
				animation_channel_partition partition_to_split = (animation_channel_partition)item_by_id(ch, partition_to_split_id);
				if(partition_to_split==null)
					return null;

				Single l1 = position / m_time_layout_scale;
				if(snap_to_frames)
					l1 = (Single)Math.Round(l1 * fps / 1000.0f) * 1000.0f / fps;

				Single l2 = partition_to_split.length / m_time_layout_scale - l1;
				partition_to_split.change_property("length", l1);
				item = new animation_channel_partition(ch, l2, partition_to_split.type);
				item_insert_index = ch.objects.IndexOf(partition_to_split) + 1;
			}
			else
			{
				Single time = position/m_time_layout_scale;
				item = new animation_channel_event(ch, time, 0);
				item_insert_index = ch.objects.Count;
			}

			add_item(channel_name, item, item_insert_index);
			return item;
		}
		public		void	add_channel				(animation_channel ch)														
		{
			if(m_channels!=null && m_channels.Contains(ch))
				return;

			ch.panel = this;
			m_channels.Add(ch);
		}
		public		void	remove_channel			(String channel_name)														
		{
			animation_channel ch = channel_by_name(channel_name);
			if(ch==null)
				return;

			m_channels.Remove(ch);
		}
		public		void	remove_channel			(animation_channel ch)														
		{
			if(m_channels==null || !m_channels.Contains(ch))
				return;

			m_channels.Remove(ch);
		}
		public		void	add_item				(String ch_name, animation_channel_item i, int index)						
		{
			animation_channel ch = channel_by_name(ch_name);
			if(ch==null)
				return;

			ch.insert(i, index);
			if(snap_to_frames)
				i.snap_to_frames();
		}
		public		void	remove_item				(String channel_name, Guid item_id)											
		{
			animation_channel ch = channel_by_name(channel_name);
			if(ch==null)
				return;

			animation_channel_item item = item_by_id(ch, item_id);
			if(item==null)
				return;

			if(ch.type==animation_channel_type.partitions)
			{
				animation_channel_partition partition = (animation_channel_partition)item;
				int my_index = ch.objects.IndexOf(partition);
				if(my_index==0)
				{
					animation_channel_partition right = (animation_channel_partition)ch.objects[1];
					right.change_property("length", (right.length + partition.length) / m_time_layout_scale);
				}
				else
				{
					animation_channel_partition left = (animation_channel_partition)ch.objects[my_index-1];
					left.change_property("length", (left.length + partition.length) / m_time_layout_scale);
				}
			}
			ch.remove(item);
		}


		#endregion

	}
}
