////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public class animation_lexeme_item: INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;
		public animation_lexeme_item()
		{
			m_cycle_from_item_index = -1;
			m_intervals = new ObservableCollection<animation_interval_item>();
		}

		private int m_cycle_from_item_index;
		public int cycle_from_item_index
		{
			get
			{
				return m_cycle_from_item_index;
			}
			set
			{
				m_cycle_from_item_index = value;
				panel.on_lexeme_modified();
				on_property_changed("cycle_from_item_index");
				on_property_changed("cycle_from_pos");
			}
		}
		public Single cycle_from_pos
		{
			get
			{
				if(m_cycle_from_item_index>=m_intervals.Count)
					m_cycle_from_item_index = -1;

				if(m_cycle_from_item_index==-1)
					return -20.0f;

				Single pos = -5.0f * panel.time_layout_scale;
				for(int i=0; i<m_cycle_from_item_index; i++)
					pos += m_intervals[i].length;

				pos += m_intervals[(int)m_cycle_from_item_index].length / 2;
				return pos;
			}
		}
		internal	animation_lexeme_panel			panel								
		{
			get;set;
		}
		public Single length
		{
			get
			{
				return (intervals_length>panel.max_time_scaled)?intervals_length:panel.max_time_scaled;
			}
		}
		public Single intervals_length
		{
			get
			{
				Single length = 0.0f;
				Single panel_max_time_scaled = panel.max_time_scaled;
				foreach(var interval in m_intervals)
					length += interval.length;

				return length;
			}
		}

		private		ObservableCollection<animation_interval_item>	m_intervals;

		public	ObservableCollection<animation_interval_item> intervals
		{
		    get
		    {
		        return m_intervals;
		    }
		}

		internal void update()
		{
			foreach(var item in m_intervals)
				item.number = UInt32.MaxValue;

			for(int index=0; index<m_intervals.Count; ++index)
			{
				if(m_intervals[index].number==UInt32.MaxValue)
					m_intervals[index].number = (UInt32)index;
			}
			on_property_changed("length");
			on_property_changed("cycle_from_item_index");
			on_property_changed("cycle_from_pos");
		}
		protected	void	on_property_changed	(String property_name)					
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property_name));
        }
		public void insert_interval(animation_interval_item item, int index)
		{
			m_intervals.Insert(index, item);
			update();
		}
		public void remove_interval(int index)
		{
			animation_interval_item item = m_intervals[index];
			if(m_cycle_from_item_index>index)
				--m_cycle_from_item_index;

			m_intervals.RemoveAt(index);
			item.number = UInt32.MaxValue;
			update();
		}
		public void remove_all_interval_views(animation_interval_item item)
		{
			ObservableCollection<animation_interval_item> lst = new ObservableCollection<animation_interval_item>(m_intervals);
			foreach(var i in lst)
			{
				if(item==i)
					m_intervals.Remove(i);
			}

			item.number = UInt32.MaxValue;
			update();
		}
	}
}
