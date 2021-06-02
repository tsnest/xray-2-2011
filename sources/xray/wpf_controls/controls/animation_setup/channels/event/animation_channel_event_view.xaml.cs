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
	public partial class animation_channel_event_view : UserControl
	{
		public animation_channel_event_view()			
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_event = (animation_channel_event)DataContext;
			};
			m_context_menu = (ContextMenu)FindResource("event_context_menu");
		}

		private	readonly	ContextMenu						m_context_menu;
		private				animation_channel_event			m_event;
		private				Boolean							m_event_mouse_down;
		private				Point							m_last_mouse_position;
		private				Single							m_event_previous_time;

		private void aaaa_type_click						(Object o, RoutedEventArgs e)		
		{
			m_event.type = 0;
		}
		private void bbbb_type_click						(Object o, RoutedEventArgs e)		
		{
			m_event.type = 1;
		}
		private void cccc_type_click						(Object o, RoutedEventArgs e)		
		{
			m_event.type = 2;
		}
		private void dddd_type_click						(Object o, RoutedEventArgs e)		
		{
			m_event.type = 3;
		}
		private void user_control_mouse_move				(Object o, MouseEventArgs e)		
		{
			if(m_event_mouse_down)
			{
				Single offset = (Single)(e.GetPosition(this).X - m_last_mouse_position.X);
				if(offset==0.0f)
					return;

			    m_event.change_property("time", (m_event.time + offset) / m_event.channel.panel.time_layout_scale);
				m_last_mouse_position = e.GetPosition(this);
			}
		}
		private void user_control_mouse_left_button_up		(Object o, MouseButtonEventArgs e)	
		{
			if(m_event.channel.panel.snap_to_frames)
			{
				foreach(animation_channel_event ev in m_event.channel.objects)
					ev.snap_to_frames();
			}

		    Single new_time = m_event.time / m_event.channel.panel.time_layout_scale;
			m_event.change_property("time", m_event_previous_time);
			m_event.time = new_time;

			m_event_mouse_down = false;
			ReleaseMouseCapture();
		}
		private void border_mouse_left_button_down			(Object o, MouseButtonEventArgs e)	
		{
			e.Handled = true;	
			m_event_mouse_down = true;
			m_last_mouse_position = e.GetPosition(this);
			m_event_previous_time = m_event.time / m_event.channel.panel.time_layout_scale;
			CaptureMouse();
		}
		private void mouse_right_button_down				(Object o, MouseButtonEventArgs e)	
		{
			m_context_menu.IsOpen = true;
			e.Handled = true;
		}
		private void remove_event_click						(Object o, RoutedEventArgs e)		
		{
			m_event.channel.panel.try_remove_item(m_event.channel.name, m_event.id);
		}
		private void remove_channel_click					(Object o, RoutedEventArgs e)		
		{
			m_event.channel.panel.try_remove_channel(m_event.channel.name);
		}
		private void copy_channel_click						(Object o, RoutedEventArgs e)		
		{
			m_event.channel.panel.try_copy_channel(m_event.channel.name);
		}
		private void cut_channel_click						(Object o, RoutedEventArgs e)		
		{
			m_event.channel.panel.try_cut_channel(m_event.channel.name);
		}
	}
}
