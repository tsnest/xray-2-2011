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
	public partial class animation_interval_view : UserControl
	{
		public animation_interval_view()
		{
			InitializeComponent();
			m_context_menu = (ContextMenu)FindResource("context_menu");
			
			DataContextChanged += delegate
			{
				m_interval = (animation_interval_item)DataContext;

				Binding binding	= new Binding("start_time");
				binding.Mode = BindingMode.OneWay;
				binding.UpdateSourceTrigger	= UpdateSourceTrigger.PropertyChanged;
				((FrameworkElement)TemplatedParent).SetBinding(Canvas.LeftProperty, binding);
			};
		}
		private				animation_interval_item		m_interval;
		private	readonly	ContextMenu					m_context_menu;

		private void user_control_mouse_right_button_down(Object o, MouseButtonEventArgs e )
		{
			if(VisualTreeHelper.GetParent((FrameworkElement)this.TemplatedParent) is StackPanel)
			{
				((MenuItem)m_context_menu.Items[3]).Visibility = Visibility.Collapsed;
				animation_lexeme_item l = m_interval.parent.panel.lexeme;
				if(l.intervals.IndexOf(m_interval)==l.cycle_from_item_index)
				{
					((MenuItem)m_context_menu.Items[0]).Visibility = Visibility.Collapsed;
					((MenuItem)m_context_menu.Items[1]).Visibility = Visibility.Visible;
				}
				else
				{
					((MenuItem)m_context_menu.Items[0]).Visibility = Visibility.Visible;
					((MenuItem)m_context_menu.Items[1]).Visibility = Visibility.Collapsed;
				}
			}
			else
			{
				((MenuItem)m_context_menu.Items[0]).Visibility = Visibility.Collapsed;
				((MenuItem)m_context_menu.Items[1]).Visibility = Visibility.Collapsed;
				((MenuItem)m_context_menu.Items[3]).Visibility = Visibility.Visible;
			}

			if(m_interval.number==UInt32.MaxValue)
				((MenuItem)m_context_menu.Items[2]).Visibility = Visibility.Collapsed;
			else
				((MenuItem)m_context_menu.Items[2]).Visibility = Visibility.Visible;

			m_context_menu.IsOpen = true;
			e.Handled = true;
		}
		private void remove_interval_click(Object o, RoutedEventArgs e)
		{
			DependencyObject obj = VisualTreeHelper.GetParent((FrameworkElement)this.TemplatedParent);
			if(obj is Canvas)
			    m_interval.parent.panel.lexeme.remove_all_interval_views(m_interval);
			else
			{
				StackPanel p = (StackPanel)obj;
				int index = p.Children.IndexOf((UIElement)this.TemplatedParent);
			    m_interval.parent.panel.lexeme.remove_interval(index);
			}
		}
		private void remove_animation_click(Object o, RoutedEventArgs e)
		{
			m_interval.parent.panel.remove_animation(m_interval.parent);
		}

		private void user_control_mouse_left_button_down(Object o, MouseButtonEventArgs mouse_button_event_args)
		{
			DependencyObject obj = VisualTreeHelper.GetParent((FrameworkElement)this.TemplatedParent);
			if(obj is StackPanel)
			{
				StackPanel p = (StackPanel)obj;
				int index = p.Children.IndexOf((UIElement)this.TemplatedParent);
				DragDrop.DoDragDrop(this, index, DragDropEffects.Move);
			}
			else
				DragDrop.DoDragDrop(this, m_interval, DragDropEffects.Link);
		}

		private void cycle_click(Object o, RoutedEventArgs e)
		{
			StackPanel p = (StackPanel)VisualTreeHelper.GetParent((FrameworkElement)this.TemplatedParent);
			int index = p.Children.IndexOf((UIElement)this.TemplatedParent);
			m_interval.parent.panel.lexeme.cycle_from_item_index = index;
		}

		private void uncycle_click(Object o, RoutedEventArgs e)
		{
			StackPanel p = (StackPanel)VisualTreeHelper.GetParent((FrameworkElement)this.TemplatedParent);
			m_interval.parent.panel.lexeme.cycle_from_item_index = -1;
		}
	}
}
