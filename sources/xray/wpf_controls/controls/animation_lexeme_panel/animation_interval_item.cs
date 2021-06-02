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
using xray.editor.wpf_controls.animation_setup;

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public class animation_interval_item: INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;
		public animation_interval_item(animation_item i, Single st, Single l, UInt32 num, animation_channel_partition_type t)
		{
			m_parent = i;
			m_parent.intervals.Add(this);
			m_start_time = st;
			m_length = l;
			m_number = num;
			m_type = t;
		}

		private		animation_item		m_parent;
		private		Single				m_start_time;
		private		Single				m_length;
		private		UInt32				m_number;
		private		animation_channel_partition_type m_type;

		public		animation_item		parent								
		{
			get
			{
				return m_parent;
			}
		}
		public		Single				start_time							
		{
			get
			{
				return m_start_time * m_parent.panel.time_layout_scale;
			}
		}
		public		Single				length								
		{
			get
			{
				return m_length * m_parent.panel.time_layout_scale;
			}
		}
		public		UInt32				number								
		{
			get
			{
				return m_number;
			}
			set
			{
				m_number = value;
				on_property_changed("number");
				on_property_changed("text");
			}
		}
		public		String				text								
		{
			get
			{
				if(m_number == UInt32.MaxValue)
					return "";
				else
					return m_number.ToString();
			}
		}
		public		animation_channel_partition_type	type				
		{
			get
			{
				return m_type;
			}
			set
			{
				m_type = value;
				on_property_changed("type");
			}
		}
		public		void				update			()					
		{
			on_property_changed("start_time");
			on_property_changed("length");
			on_property_changed("number");
			on_property_changed("text");
			on_property_changed("type");
		}
		protected	void	on_property_changed		(String property_name)	
        {
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(property_name));
        }
	}
}
