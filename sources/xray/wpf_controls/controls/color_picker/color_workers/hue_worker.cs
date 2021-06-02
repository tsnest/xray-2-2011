////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.color_picker
{
	internal class hue_worker: color_part_worker_base
	{
		color_hsv m_hsv_color;

		public override			Double		color_to_part		( color_hsv color )
		{
			m_hsv_color = color;
			return 360 - m_hsv_color.h;
		}
		public override			color_hsv	part_to_color		( Double part )
		{
			m_hsv_color.h = 360 - part;
			return m_hsv_color;
		}
	}
}
