////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Reflection;

namespace xray.editor.wpf_controls
{
	public interface i_property_info_value: i_property_value
	{
		PropertyInfo		property_info		
		{
			get;
		}
	}
}
