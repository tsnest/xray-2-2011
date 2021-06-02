////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace xray.editor.wpf_controls.animation_setup
{
	public class animation_channel_interval : animation_channel_item
	{
        public		animation_channel_interval(animation_channel ch, Single st, Single l, UInt32 t)	
			:base(ch)
        {
			m_start_time = st;
			m_length = l;
			m_type = t;
        }

		private		Single				m_start_time;
		private		Single				m_length;
		private		UInt32				m_type;

		public		Single				start_time			
		{
			get
			{
				return m_start_time * channel.panel.time_layout_scale;
			}
			set
			{
				channel.panel.try_change_item_property(channel.name, id, "start_time", value);
			}
		}
		public		Single				length				
		{
			get
			{
				return m_length * channel.panel.time_layout_scale;
			}
			set
			{
				channel.panel.try_change_item_property(channel.name, id, "length", value);
			}
		}
		public		UInt32				type				
		{
			get
			{
				return m_type;
			}
			set
			{
				channel.panel.try_change_item_property(channel.name, id, "type", value);
			}
		}
		public		Single				center_length		
		{
			get
			{
				return length - 6;
			}
		}

		public override	void			update			()	
		{
			on_property_changed("start_time");
			on_property_changed("length");
			on_property_changed("center_length");
			on_property_changed("type");
		}
		public override void			snap_to_frames	()	
		{
			Single fps = channel.panel.fps;
			change_property("start_time", (Single)Math.Round(m_start_time * fps / 1000.0f) * 1000.0f / fps);
			change_property("length", (Single)Math.Round(m_length * fps / 1000.0f) * 1000.0f / fps);
		}
		public override	void			change_property	(String property_name, Object value)
		{
			if(property_name=="start_time")
			{
                m_start_time = (Single)value;
				if(m_start_time<0)
					m_start_time = 0;
			}
			else if(property_name=="type")
			{
                m_type = (UInt32)value;
			}
			else if(property_name=="length")
			{
				m_length = (Single)value;
				if(m_length<30)
					m_length = 30;

				on_property_changed("center_length");
			}

			on_property_changed(property_name);
		}
		public override	Object	get_property				(String property_name)			
		{
			if(property_name=="start_time")
                return m_start_time;
			else if(property_name=="type")
                return m_type;
			else if(property_name=="length")
                return m_length;

			return null;
		}
	}
}
