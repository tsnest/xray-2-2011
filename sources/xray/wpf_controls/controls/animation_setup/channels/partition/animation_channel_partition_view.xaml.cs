////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
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
	public partial class animation_channel_partition_view : UserControl
	{
		public	animation_channel_partition_view()	
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_partition = (animation_channel_partition)DataContext;
			};
			m_partition_context_menu = (ContextMenu)FindResource("partition_context_menu");
		}

		private animation_channel_partition		m_partition;
		private readonly ContextMenu			m_partition_context_menu;
		private Point							m_right_mouse_button_down_point;
		private Point							m_last_mouse_position;
		private Boolean							m_right_edge_mouse_down;
		private Boolean							m_center_mouse_down;
		private Single							m_previous_length1;
		private Single							m_previous_length2;

		private void	none_click								(Object o, RoutedEventArgs e)		
		{
			m_partition.type = animation_channel_partition_type.none;
		}
		private void	left_click								(Object o, RoutedEventArgs e)		
		{
			m_partition.type = animation_channel_partition_type.left;
		}
		private void	right_click								(Object o, RoutedEventArgs e)		
		{
			m_partition.type = animation_channel_partition_type.right;
		}
		private void	both_click								(Object o, RoutedEventArgs e)		
		{
			m_partition.type = animation_channel_partition_type.both;
		}
		private void	user_control_mouse_right_button_down	(Object o, MouseButtonEventArgs e)	
		{
			m_right_mouse_button_down_point = Mouse.GetPosition(this);
			if(m_partition.channel.objects.Count>1)
			{
				MenuItem item = (MenuItem)m_partition_context_menu.Items[2];
				item.IsEnabled = true;
			}
			else
			{
				MenuItem item = (MenuItem)m_partition_context_menu.Items[2];
				item.IsEnabled = false;
			}

			MenuItem change_type_item = (MenuItem)m_partition_context_menu.Items[0];
			Int32 i = -1;
			if(m_partition.type==animation_channel_partition_type.none)
				i = 0;
			else if(m_partition.type==animation_channel_partition_type.left)
				i = 1;
			else if(m_partition.type==animation_channel_partition_type.right)
				i = 2;
			else if(m_partition.type==animation_channel_partition_type.both)
				i = 3;

			for(Int32 index=0; index<change_type_item.Items.Count; ++index)
			{
				MenuItem menu_item = (MenuItem)change_type_item.Items[index];
				if(i==index)
					menu_item.IsChecked = true;
				else
					menu_item.IsChecked = false;
			}

			m_partition_context_menu.IsOpen = true;
			e.Handled = true;
		}
		private void	right_edge_mouse_left_button_down		(Object o, MouseButtonEventArgs e)	
		{
			e.Handled = true;	
			m_last_mouse_position = e.GetPosition(this);
			m_right_edge_mouse_down = true;
			CaptureMouse();

			int my_index = m_partition.channel.objects.IndexOf(m_partition);
			if(my_index==m_partition.channel.objects.Count-1)
				return;

			animation_channel_partition right = (animation_channel_partition)m_partition.channel.objects[my_index+1];
			m_previous_length1 = right.length / m_partition.channel.panel.time_layout_scale;
			m_previous_length2 = m_partition.length / m_partition.channel.panel.time_layout_scale;
		}
		private void	center_mouse_left_button_down			(Object o, MouseButtonEventArgs e)	
		{
			e.Handled = true;	
			m_last_mouse_position = e.GetPosition(this);
			m_center_mouse_down = true;
			CaptureMouse();

			int my_index = m_partition.channel.objects.IndexOf(m_partition);
			if(my_index==m_partition.channel.objects.Count-1 || my_index==0)
				return;

			animation_channel_partition left = (animation_channel_partition)m_partition.channel.objects[my_index-1];
			animation_channel_partition right = (animation_channel_partition)m_partition.channel.objects[my_index+1];
			m_previous_length1 = left.length / m_partition.channel.panel.time_layout_scale;
			m_previous_length2 = right.length / m_partition.channel.panel.time_layout_scale;
		}
		private void	user_control_mouse_move					(Object o, MouseEventArgs e)		
		{
			if(!m_right_edge_mouse_down && !m_center_mouse_down)
				return;

			Single offset = (Single)(e.GetPosition(this).X - m_last_mouse_position.X);
			if(offset==0.0f)
				return;

			float current_scale = m_partition.channel.panel.time_layout_scale;
			if(m_right_edge_mouse_down)
			{
				int my_index = m_partition.channel.objects.IndexOf(m_partition);
				if(my_index==m_partition.channel.objects.Count-1)
					return;

				animation_channel_partition right = (animation_channel_partition)m_partition.channel.objects[my_index+1];
				Single new_my_length = (m_partition.length + offset) / current_scale;
				Single new_right_length = (right.length - offset) / current_scale;
				if(m_partition.channel.panel.snap_to_frames)
				{
					Single frame_length = 1000/m_partition.channel.panel.fps; 
					if(new_my_length<=frame_length && offset<0)
						return;

					if(new_right_length<=frame_length && offset>0)
						return;
				}
				else
				{
					if(new_my_length<=1 && offset<0)
						return;
		
					if(new_right_length<=1 && offset>0)
						return;
				}

				right.change_property("length", new_right_length);
				m_partition.change_property("length", new_my_length);
				m_last_mouse_position = e.GetPosition(this);
			}
			else if(m_center_mouse_down)
			{
				int my_index = m_partition.channel.objects.IndexOf(m_partition);
				if(my_index==m_partition.channel.objects.Count-1 || my_index==0)
					return;

				animation_channel_partition left = (animation_channel_partition)m_partition.channel.objects[my_index-1];
				animation_channel_partition right = (animation_channel_partition)m_partition.channel.objects[my_index+1];
				Single new_left_length = (left.length + offset) / current_scale;
				Single new_right_length = (right.length - offset) / current_scale;
				if(m_partition.channel.panel.snap_to_frames)
				{
					Single frame_length = 1000/m_partition.channel.panel.fps; 
					if(new_left_length<=frame_length && offset<0)
						return;

					if(new_right_length<=frame_length && offset>0)
						return;
				}
				else
				{
					if(new_left_length<=1 && offset<0)
						return;
		
					if(new_right_length<=1 && offset>0)
						return;
				}

				left.change_property("length", new_left_length);
				right.change_property("length", new_right_length);
			}
		}
		private void	user_control_mouse_left_button_up_impl	()									
		{
			if(m_right_edge_mouse_down)
			{
				int my_index = m_partition.channel.objects.IndexOf(m_partition);
				if(my_index==m_partition.channel.objects.Count-1)
					return;

				animation_channel_partition right = (animation_channel_partition)m_partition.channel.objects[my_index+1];
				List<Guid> item_ids = new List<Guid>();
				List<String> prop_names = new List<String>();
				List<Object> new_vals = new List<Object>();
				item_ids.Add(right.id);
				item_ids.Add(m_partition.id);
				prop_names.Add("length");
				prop_names.Add("length");
				new_vals.Add(right.length / m_partition.channel.panel.time_layout_scale);
				new_vals.Add(m_partition.length / m_partition.channel.panel.time_layout_scale);
				right.change_property("length", m_previous_length1);
				m_partition.change_property("length", m_previous_length2);
				m_partition.channel.panel.try_change_item_properties(m_partition.channel.name, item_ids, prop_names, new_vals);
			}
			else if(m_center_mouse_down)
			{
				int my_index = m_partition.channel.objects.IndexOf(m_partition);
				if(my_index==m_partition.channel.objects.Count-1 || my_index==0)
					return;

				animation_channel_partition left = (animation_channel_partition)m_partition.channel.objects[my_index-1];
				animation_channel_partition right = (animation_channel_partition)m_partition.channel.objects[my_index+1];
				List<Guid> item_ids = new List<Guid>();
				List<String> prop_names = new List<String>();
				List<Object> new_vals = new List<Object>();
				item_ids.Add(left.id);
				item_ids.Add(right.id);
				prop_names.Add("length");
				prop_names.Add("length");
				new_vals.Add(left.length / m_partition.channel.panel.time_layout_scale);
				new_vals.Add(right.length / m_partition.channel.panel.time_layout_scale);
				left.change_property("length", m_previous_length1);
				right.change_property("length", m_previous_length2);
				m_partition.channel.panel.try_change_item_properties(m_partition.channel.name, item_ids, prop_names, new_vals);
			}
		}
		private void	user_control_mouse_left_button_up		(Object o, MouseButtonEventArgs e)	
		{
			if(m_partition.channel.panel.snap_to_frames)
			{
				foreach(animation_channel_partition partition in m_partition.channel.objects)
					partition.snap_to_frames();
			}

			user_control_mouse_left_button_up_impl();
			m_right_edge_mouse_down = false;
			m_center_mouse_down = false;
			ReleaseMouseCapture();
		}
		private void	split_click								(Object o, RoutedEventArgs e)		
		{
			m_partition.channel.panel.try_add_item(m_partition.channel.name, (Single)m_right_mouse_button_down_point.X, m_partition.id);
		}
		private void	remove_partition_click					(Object o, RoutedEventArgs e)		
		{
			m_partition.channel.panel.try_remove_item(m_partition.channel.name, m_partition.id);
		}
		private void	remove_channel_click					(Object o, RoutedEventArgs e)		
		{
			m_partition.channel.panel.try_remove_channel(m_partition.channel.name);
		}
		private void	copy_channel_click						(Object o, RoutedEventArgs e)		
		{
			m_partition.channel.panel.try_copy_channel(m_partition.channel.name);
		}
		private void	cut_channel_click						(Object o, RoutedEventArgs e)		
		{
			m_partition.channel.panel.try_cut_channel(m_partition.channel.name);
		}
	}
}