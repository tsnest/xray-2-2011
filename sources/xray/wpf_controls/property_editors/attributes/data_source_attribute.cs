////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class data_source_attribute: Attribute
	{
		public		data_source_attribute ()
		{
		}
		public		data_source_attribute ( String data_source_type, Func<String, IEnumerable<String>> get_items, List<String> input_values_list, Action<String> input_value_setter, Func<String> input_value_getter )         
		{
			this.get_items			= get_items;
			this.data_source_type	= data_source_type;
			this.input_value_setter = input_value_setter;
			this.input_value_getter = input_value_getter;
			this.input_values_list	= input_values_list;

		}
		
		public readonly	String								data_source_type;
		public readonly Func<String, IEnumerable<String>>	get_items;
		public readonly Action<String>						input_value_setter;
		public readonly Func<String>						input_value_getter;
		public readonly List<String>						input_values_list;
	}
}
