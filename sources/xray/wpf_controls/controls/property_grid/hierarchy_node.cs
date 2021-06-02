////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.property_grid
{
	internal	class		hierarchy_node		
	{
		public				String					name;
		public readonly		List<hierarchy_node>	sub_nodes = new List<hierarchy_node>( );

		public override		String					ToString		( )		
		{
			return name;
		}
	}
}
