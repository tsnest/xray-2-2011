////////////////////////////////////////////////////////////////////////////
//	Created		: 05.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class custom_property_attribute: Attribute
	{
		public custom_property_attribute( Func<List<String>> box_values_getter, Func<String> box_value_getter, Action<String> box_value_setter )
		{
			this.box_values_getter				= box_values_getter;	
			this.box_value_getter		= box_value_getter;
			this.box_value_setter		= box_value_setter;
		}

		public		Func<List<String>>	box_values_getter;
		public		Func<String>		box_value_getter;
		public		Action<String>		box_value_setter;
	}
}
