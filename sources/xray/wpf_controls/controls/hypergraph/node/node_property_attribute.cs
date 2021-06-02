////////////////////////////////////////////////////////////////////////////
//	Created		: 14.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.hypergraph
{
	public class node_property_attribute: Attribute
	{
		public node_property_attribute( )
		{
			input_link_enabled	= true;
			output_link_enabled	= true;
		}

		public node_property_attribute( bool input_enabled, bool output_enabled )
		{
			input_link_enabled	= input_enabled;
			output_link_enabled	= output_enabled;
		}

		public String		type_id					
		{
			get;
			set;
		}
		public Boolean		input_link_enabled		
		{
			get;
			set;
		}
		public Boolean		output_link_enabled		
		{
			get;
			set;
		}
	}

	public class node_sub_properties_attribute: Attribute
	{
		public node_sub_properties_attribute( )
		{
			input_link_enabled	= true;
			output_link_enabled	= true;
		}

		public node_sub_properties_attribute( bool input_enabled, bool output_enabled )
		{
			input_link_enabled	= input_enabled;
			output_link_enabled	= output_enabled;
		}

		public String		type_id					
		{
			get;
			set;
		}
		public Boolean		input_link_enabled		
		{
			get;
			set;
		}
		public Boolean		output_link_enabled		
		{
			get;
			set;
		}
	}
}
