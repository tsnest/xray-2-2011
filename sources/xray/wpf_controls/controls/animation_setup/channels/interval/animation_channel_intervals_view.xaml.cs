////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System;
using System.Windows.Media;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace xray.editor.wpf_controls.animation_setup
{
	public partial class animation_channel_intervals_view
	{
		public animation_channel_intervals_view()	
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_channel = (animation_channel)DataContext;
				m_intervals_view.ItemsSource = m_channel.objects;
			};
			m_context_menu = (ContextMenu)FindResource("view_context_menu");
		}

		private			animation_channel	m_channel;
		private readonly ContextMenu		m_context_menu;
		private			Point				m_last_mouse_down_position;

		private void	mouse_right_button_down	(Object o, MouseButtonEventArgs e)	
		{
			m_context_menu.IsOpen = true;
			m_last_mouse_down_position = Mouse.GetPosition(this);
			e.Handled = true;
		}
		private void	add_new_interval_click	(Object sender, RoutedEventArgs e)	
		{
			m_channel.panel.try_add_item(m_channel.name, (Single)(m_last_mouse_down_position.X), Guid.Empty);
		}
		private void	remove_channel_click	(Object sender, RoutedEventArgs e)	
		{
			m_channel.panel.try_remove_channel(m_channel.name);
		}
		private void	copy_channel_click		(Object o, RoutedEventArgs e)		
		{
			m_channel.panel.try_copy_channel(m_channel.name);
		}
		private void	cut_channel_click		(Object o, RoutedEventArgs e)		
		{
			m_channel.panel.try_cut_channel(m_channel.name);
		}
	}
}
