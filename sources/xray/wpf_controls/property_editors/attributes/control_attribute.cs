////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using xray.editor.wpf_controls.control_containers;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class control_attribute: Attribute
	{
		public	control_attribute	( control control )
		{
			m_control	= control;
		}

		public		control		m_control;
	}
}
