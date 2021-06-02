////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
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
    public partial class animation_channel_events_view : UserControl
    {
 		public animation_channel_events_view()	
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_channel = (animation_channel)DataContext;
				m_events_view.ItemsSource = m_channel.objects;
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
		private void	add_new_event_click		(Object sender, RoutedEventArgs e)	
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
