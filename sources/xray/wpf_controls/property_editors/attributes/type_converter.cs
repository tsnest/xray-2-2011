////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public abstract class type_converter_base
	{
		public abstract		Type	base_type		
		{
			get;
		}
		public abstract		Type	converted_type	
		{
			get;
		}

		public abstract		Object	convert			( Object obj );
		public abstract		Object	convert_back	( Object obj );
	}
}
