////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace xray.editor.wpf_controls.animation_setup
{
	public partial class animation_channel_interval_view : UserControl
	{
		public animation_channel_interval_view()			
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_interval = (animation_channel_interval)DataContext;
			};
			m_context_menu = (ContextMenu)FindResource("interval_context_menu");
		}

		private	readonly	ContextMenu						m_context_menu;
		private				animation_channel_interval		m_interval;
		private				Boolean							m_left_edge_mouse_down;
		private				Boolean							m_right_edge_mouse_down;
		private				Boolean							m_center_mouse_down;
		private				Point							m_last_mouse_position;
		private				Single							m_interval_previous_pos;
		private				Single							m_interval_previous_length;

		private void left_edge_mouse_left_button_down		(Object o, MouseButtonEventArgs e)	
		{
			m_interval_previous_pos = m_interval.start_time / m_interval.channel.panel.time_layout_scale;
			m_interval_previous_length = m_interval.length / m_interval.channel.panel.time_layout_scale;
			e.Handled = true;	
			m_left_edge_mouse_down = true;
			CaptureMouse();
		}
		private void right_edge_mouse_left_button_down		(Object o, MouseButtonEventArgs e)	
		{
			m_interval_previous_pos = m_interval.start_time / m_interval.channel.panel.time_layout_scale;
			m_interval_previous_length = m_interval.length / m_interval.channel.panel.time_layout_scale;
			e.Handled = true;	
			m_right_edge_mouse_down = true;
			CaptureMouse();
		}
		private void user_control_mouse_move				(Object o, MouseEventArgs e)		
		{
			Single offset = (Single)(e.GetPosition(this).X - m_last_mouse_position.X);
			if(offset==0.0f)
				return;

			int my_index = m_interval.channel.objects.IndexOf(m_interval);
			animation_channel_interval left = (my_index>0) ? 
				(animation_channel_interval)m_interval.channel.objects[my_index-1]
				: null;
			animation_channel_interval right = (my_index<m_interval.channel.objects.Count-1) ? 
				(animation_channel_interval)m_interval.channel.objects[my_index+1]
				: null;
			float current_scale = m_interval.channel.panel.time_layout_scale;
			if(m_left_edge_mouse_down)
			{
				Single new_start_time = (m_interval.start_time + offset) / current_scale;
				Single new_length = (m_interval.length - offset) / current_scale;
				if(offset<0)
				{
					if(left!=null && new_start_time<(left.start_time+left.length)/current_scale)
					{
						new_start_time = (left.start_time+left.length)/current_scale;
						new_length = (m_interval.start_time+m_interval.length)/current_scale-new_start_time;
					}
				}
				else
				{
					if(right!=null && new_start_time+new_length>right.start_time/current_scale)
					{
						new_start_time = (right.start_time-m_interval.length)/current_scale;
						new_length = (right.start_time)/current_scale-new_start_time;
					}
				}

				if(new_start_time<0)
				{
					new_length += new_start_time;
					new_start_time = 0;
				}
				if(new_length<30)
				{
					new_length = 30;
					new_start_time = (m_interval.start_time+m_interval.length)/current_scale - new_length;
				}

				m_interval.change_property("start_time", new_start_time);
				m_interval.change_property("length", new_length);
			}
			else if(m_right_edge_mouse_down)
			{
				Single new_length = (m_interval.length + offset) / current_scale;
				if(offset>0 && right!=null)
				{
					if(new_length+m_interval.start_time/current_scale>right.start_time/current_scale)
						new_length = (right.start_time-m_interval.start_time)/current_scale;
				}
				
				if(new_length+m_interval.start_time/current_scale>m_interval.channel.length/current_scale)
					new_length = (m_interval.channel.length-m_interval.start_time)/current_scale;

				m_interval.change_property("length", new_length);
			}
			else if(m_center_mouse_down)
			{
				Single new_start_time = (m_interval.start_time + offset) / current_scale;
				if(offset<0)
				{
					if(left!=null && new_start_time<(left.start_time+left.length)/current_scale)
						new_start_time = (left.start_time+left.length)/current_scale;
				}
				else
				{
					if(right!=null && new_start_time+m_interval.length/current_scale>right.start_time/current_scale)
						new_start_time = (right.start_time-m_interval.length)/current_scale;

					if(new_start_time+m_interval.length/current_scale>m_interval.channel.length/current_scale)
						new_start_time = (m_interval.channel.length-m_interval.length)/current_scale;
				}

				m_interval.change_property("start_time", new_start_time);
			}

			m_last_mouse_position = e.GetPosition(this);
		}
		private void user_control_mouse_left_button_down	(Object o, MouseButtonEventArgs e)	
		{
			m_last_mouse_position = e.GetPosition(this);
		}
		private void user_control_mouse_left_button_up		(Object o, MouseButtonEventArgs e)	
		{
			if(m_interval.channel.panel.snap_to_frames)
			{
				foreach(animation_channel_interval interval in m_interval.channel.objects)
					interval.snap_to_frames();
			}

			Single new_interval_pos = m_interval.start_time / m_interval.channel.panel.time_layout_scale;
			Single new_interval_length = m_interval.length / m_interval.channel.panel.time_layout_scale;
			m_interval.change_property("start_time", m_interval_previous_pos);
			m_interval.change_property("length", m_interval_previous_length);

			List<Guid> item_ids = new List<Guid>();
			item_ids.Add(m_interval.id);
			item_ids.Add(m_interval.id);

			List<String> prop_names = new List<String>();
			prop_names.Add("start_time");
			prop_names.Add("length");
	
			List<Object> new_vals = new List<Object>();
			new_vals.Add(new_interval_pos);
			new_vals.Add(new_interval_length);

			m_interval.channel.panel.try_change_item_properties(m_interval.channel.name, item_ids, prop_names, new_vals);

			m_left_edge_mouse_down = false;
			m_right_edge_mouse_down = false;
			m_center_mouse_down = false;
			ReleaseMouseCapture();
		}
		private void border_mouse_left_button_down			(Object o, MouseButtonEventArgs e)	
		{
			m_interval_previous_pos = m_interval.start_time / m_interval.channel.panel.time_layout_scale;
			m_interval_previous_length = m_interval.length / m_interval.channel.panel.time_layout_scale;
			e.Handled = true;	
			m_center_mouse_down = true;
			CaptureMouse();
		}
		private void mouse_right_button_down				(Object o, MouseButtonEventArgs e)	
		{
			m_context_menu.IsOpen = true;
			e.Handled = true;
		}
		private void remove_interval_click					(Object o, RoutedEventArgs e)		
		{
			m_interval.channel.panel.try_remove_item(m_interval.channel.name, m_interval.id);
		}
		private void remove_channel_click					(Object o, RoutedEventArgs e)		
		{
			m_interval.channel.panel.try_remove_channel(m_interval.channel.name);
		}
		private void copy_channel_click						(Object o, RoutedEventArgs e)		
		{
			m_interval.channel.panel.try_copy_channel(m_interval.channel.name);
		}
		private void cut_channel_click						(Object o, RoutedEventArgs e)		
		{
			m_interval.channel.panel.try_cut_channel(m_interval.channel.name);
		}
	}
}
