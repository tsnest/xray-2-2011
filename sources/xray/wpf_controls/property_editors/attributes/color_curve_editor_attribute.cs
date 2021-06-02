////////////////////////////////////////////////////////////////////////////
//	Created		: 08.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class color_curve_editor_attribute: Attribute
	{
		internal	Action<Double>	limit_setter;
		public		Func<Boolean>	recalculate_keys = ( ) => true;
		
		public		void			set_limit		( Double limit )	
		{
			limit_setter	( limit );
		}
	}
}
