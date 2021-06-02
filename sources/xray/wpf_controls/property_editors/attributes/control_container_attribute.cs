////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using xray.editor.wpf_controls.control_containers;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class control_container_attribute: Attribute
	{
		public	control_container_attribute	( control_container control_container )
		{
			m_control_container = control_container;
		}

		public		control_container		m_control_container;
	}
}
