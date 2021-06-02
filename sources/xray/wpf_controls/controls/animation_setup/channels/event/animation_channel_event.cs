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
	public class animation_channel_event : animation_channel_item
	{
        public		animation_channel_event			(animation_channel ch, Single time, UInt32 type)
			:base(ch)
        {
			m_time = time;
			m_type = type;
        }

		private		Single				m_time;
		private		UInt32				m_type;

		public		Single				time		
		{
		    get
		    {
		        return m_time * channel.panel.time_layout_scale;
		    }
		    set
		    {
				channel.panel.try_change_item_property(channel.name, id, "time", value);
		    }
		}
		public		Single				position	
		{
		    get
		    {
		        return time - 15;
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
		public		String				text		
		{
		    get
		    {
		        return m_type.ToString();
		    }
		}

		public override void			update			()	
		{
		    on_property_changed("time");
	        on_property_changed("position");
		    on_property_changed("type");
		    on_property_changed("text");
		}
		public override void			snap_to_frames	()	
		{
			Single fps = channel.panel.fps;
			change_property("time", (Single)Math.Round(m_time * fps / 1000.0f) * 1000.0f / fps);
		}
		public override	void			change_property	(String property_name, Object value)
		{
			if(property_name=="time")
			{
                m_time = (Single)value;
		        if(m_time<0)
		            m_time = 0;
			
		        if(m_time > channel.length/channel.panel.time_layout_scale)
		            m_time = channel.length/channel.panel.time_layout_scale;

				on_property_changed("position");
			}
			else if(property_name=="type")
			{
                m_type = (UInt32)value;
				on_property_changed("text");
			}

			on_property_changed(property_name);
		}
		public override	Object			get_property	(String property_name)				
		{
			if(property_name=="time")
                return m_time;
			else if(property_name=="type")
                return m_type;

			return null;
		}
	}
}
