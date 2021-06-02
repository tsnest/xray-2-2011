////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
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
using System.ComponentModel;

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public partial class animation_lexeme_item_view : UserControl, INotifyPropertyChanged
	{
		public animation_lexeme_item_view()
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_lexeme = (animation_lexeme_item)DataContext;
				m_intervals_view.ItemsSource = m_lexeme.intervals;
			};
		}
		private				animation_lexeme_item		m_lexeme;

		public				Double						tmp_interval_left
		{
			get
			{
				return (Double)m_tmp_interval.GetValue(Canvas.LeftProperty);
			}
			set
			{
				m_tmp_interval.SetValue(Canvas.LeftProperty, value);
				if(PropertyChanged != null)
					PropertyChanged(this, new PropertyChangedEventArgs("tmp_interval_left"));
			}
		}

		private void user_control_drag_leave(Object o, DragEventArgs e)
		{
			m_tmp_interval.Width = 0.0f;
		}

		private void user_control_drag_over (Object o, DragEventArgs e)
		{
			m_tmp_interval.Width = 10.0f;

			Single panel_scale = m_lexeme.panel.time_layout_scale;
			Single mouse_pos = (Single)e.GetPosition(this).X / panel_scale;
			Single tmp_interval_pos = 0.0f;

			if(m_lexeme.intervals.Count>0)
			{
			    for(int i=0; i<m_lexeme.intervals.Count; ++i)
			    {
					Single cur_interval_length = m_lexeme.intervals[i].length / panel_scale;
					if(tmp_interval_pos+cur_interval_length/2 < mouse_pos)
						tmp_interval_pos += cur_interval_length;
					else
						break;
			    }
			}

			tmp_interval_left = tmp_interval_pos*panel_scale - 5.0f;
			e.Handled = true;			
		}
		private void user_control_drop (Object o, DragEventArgs e)
		{
			m_tmp_interval.Width = 0.0f;
			if(e.Effects==DragDropEffects.Link)
			{
				Object data = e.Data.GetData(typeof(animation_interval_item));
				Single panel_scale = m_lexeme.panel.time_layout_scale;
				Single mouse_pos = (Single)e.GetPosition(this).X / panel_scale;
				Single last_item_pos = 0.0f;
				int index = 0;
				for(; index<m_lexeme.intervals.Count; ++index)
				{
					Single cur_interval_length = m_lexeme.intervals[index].length / panel_scale;
					if(last_item_pos+cur_interval_length/2 < mouse_pos)
						last_item_pos += cur_interval_length;
					else
						break;
				}

				m_lexeme.insert_interval((animation_interval_item)data, index);
			}
			else if(e.Effects==DragDropEffects.Move)
			{
				Object data = e.Data.GetData(typeof(int));
				int moving_index = (int)data;
				animation_interval_item moving_item = m_lexeme.intervals[moving_index];
				Single panel_scale = m_lexeme.panel.time_layout_scale;
				Single mouse_pos = (Single)e.GetPosition(this).X / panel_scale;
				Single last_item_pos = 0.0f;
				int index = 0;
				for(; index<m_lexeme.intervals.Count; ++index)
				{
					Single cur_interval_length = m_lexeme.intervals[index].length / panel_scale;
					if(last_item_pos+cur_interval_length/2 < mouse_pos)
						last_item_pos += cur_interval_length;
					else
						break;
				}

				if(index>moving_index)
					--index;

				if(index!=moving_index)
				{
					m_lexeme.remove_interval(moving_index);
					m_lexeme.insert_interval(moving_item, index);
				}

				e.Handled = true;
			}
		}

		#region INotifyPropertyChanged Members

		public event PropertyChangedEventHandler PropertyChanged;

		#endregion
	}
}
