////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
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

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public partial class animation_item_intervals_view : UserControl
	{
		public animation_item_intervals_view()
		{
			InitializeComponent();
			m_context_menu = (ContextMenu)FindResource("context_menu");
			DataContextChanged += delegate
			{
				m_item = (animation_item)DataContext;
				m_intervals_view.ItemsSource = m_item.intervals;
			};
		}
		private	readonly	ContextMenu			m_context_menu;
		private				animation_item		m_item;
		private void user_control_mouse_right_button_down(Object o, MouseButtonEventArgs e)
		{
			m_context_menu.IsOpen = true;
			e.Handled = true;
		}
		private void remove_animation_click(Object o, RoutedEventArgs e)
		{
			m_item.panel.remove_animation(m_item);
		}
	}
}
