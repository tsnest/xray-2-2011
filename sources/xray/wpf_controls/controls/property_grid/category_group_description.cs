////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Globalization;

namespace xray.editor.wpf_controls.property_grid
{
	internal class category_group_description : GroupDescription
	{
		public override Object GroupNameFromItem( Object item, Int32 level, CultureInfo culture )
		{
			var attribute = (CategoryAttribute)( ( (property_grid_item)item ).m_property ).descriptors[0].Attributes[typeof(CategoryAttribute)];
			if ( attribute != null )
				return attribute.Category.Trim( );

			return "Misc";
		}
	}
}
