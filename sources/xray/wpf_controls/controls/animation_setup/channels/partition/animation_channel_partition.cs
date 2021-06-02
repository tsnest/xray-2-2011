////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
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
	public class animation_channel_partition : animation_channel_item
	{
		public animation_channel_partition(animation_channel ch, Single l, animation_channel_partition_type t)
			:base(ch)
		{
			m_type		= t;
			m_length	= l;
		}

		private			Single								m_length;
		private			animation_channel_partition_type	m_type;

		public			Single								length	
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
		public			animation_channel_partition_type	type	
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

		public override void	update						()		
		{
			on_property_changed("length");
		}
		public override void	snap_to_frames				()		
		{
			Single fps = channel.panel.fps;
			change_property("length", (Single)Math.Round(m_length * fps / 1000.0f) * 1000.0f / fps);
		}
		public override	void	change_property				(String property_name, Object value)
		{
			if(property_name=="type")
                m_type = (animation_channel_partition_type)value;
			else if(property_name=="length")
                m_length = (Single)value;

			on_property_changed(property_name);
		}
		public override	Object	get_property				(String property_name)	
		{
			if(property_name=="type")
                return m_type;
			else if(property_name=="length")
                return m_length;

			return null;
		}
	}
}