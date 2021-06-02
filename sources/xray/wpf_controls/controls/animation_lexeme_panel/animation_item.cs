////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public class animation_item: INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;
		public animation_item(String n, Single l)
		{
			m_name = n;
			m_length = l;
			m_intervals = new ObservableCollection<animation_interval_item>();
		}

		private		ObservableCollection<animation_interval_item>	m_intervals;
		private		Single m_length;
		private		String m_name;

		public	ObservableCollection<animation_interval_item> intervals
		{
		    get
		    {
		        return m_intervals;
		    }
		}
		public	Single length
		{
		    get
		    {
		        return m_length * panel.time_layout_scale;
		    }
		    set
		    {
				m_length = value;
				on_property_changed("length");
		    }
		}
		public	String name
		{
		    get
		    {
		        return m_name;
		    }
		    set
		    {
				m_name = value;
				on_property_changed("name");
				on_property_changed("short_name");
		    }
		}
		public	String short_name
		{
		    get
		    {
		        return m_name.Remove(0, m_name.LastIndexOf("/")+1);
		    }
		}
		public	animation_lexeme_panel			panel								
		{
			get;set;
		}
		internal	void	update				()										
		{
			on_property_changed("name");
			on_property_changed("short_name");
			on_property_changed("length");
			foreach(var item in m_intervals)
			    item.update();
		}
		protected	void	on_property_changed	(String property_name)					
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property_name));
        }
	}
}
