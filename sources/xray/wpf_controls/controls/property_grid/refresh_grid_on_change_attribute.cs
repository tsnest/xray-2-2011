////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_grid
{
	[AttributeUsage( AttributeTargets.Property, Inherited = true)]
	public class refresh_grid_on_change_attribute: Attribute
	{
	}
}
