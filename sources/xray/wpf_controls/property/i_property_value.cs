////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public interface i_property_value
	{
		Object		get_value		( );
		void		set_value		( Object value );
		Type		value_type		{get;}
	}
}
