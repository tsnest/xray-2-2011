////////////////////////////////////////////////////////////////////////////
//	Created		: 12.03.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.color_picker
{
	internal class multiply_worker: color_part_worker_base
	{
		Double		m_value;
		color_hsv	m_color;

		public override double color_to_part( color_hsv color )
		{
			m_color = color;
			return m_value;
		}
		public override color_hsv part_to_color( double part )
		{
			m_value = part;
			return m_color;
		}
	}
}
