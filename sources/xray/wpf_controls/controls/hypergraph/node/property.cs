////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.hypergraph
{
	public class property: property_editors.property
	{
		public		property	( )		
		{
			input_enabled	= false;
			output_enabled	= false;
		}

		public				String				type_id			
		{
			get;set;
		}
		public				Boolean				input_enabled	
		{
			get;set;
		}
		public				Boolean				output_enabled	
		{
			get;set;
		}

		internal			property_view		property_view	
		{
			get;set;
		}
		public				node				node			
		{
			get;set;
		}
		public override		Object				value			
		{
			get
			{
				if( descriptors == null || descriptors.Count == 0 )
					return null;
				
				return base.value;
			}
			set
			{
				if( descriptors == null || descriptors.Count == 0 )
					return;

				base.value = value;
			}
		}

		public override		String	ToString	( )				
		{
			return value.ToString( );
		}

	}
}
