////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Collections.Generic;

namespace xray.editor.wpf_controls
{
	public interface i_property_container: IEnumerable<property_descriptor>
	{
		IEnumerable<property_descriptor>	get_properties	();
	}
}
