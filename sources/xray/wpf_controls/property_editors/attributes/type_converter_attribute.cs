////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class type_converter_attribute: Attribute
	{
		public type_converter_attribute( Type destination_type, Type converter_type )
		{
			m_destination_type	= destination_type;
			m_converter_type	= converter_type;
		}

		public readonly		Type	m_destination_type;
		public readonly		Type	m_converter_type;
	}
}
