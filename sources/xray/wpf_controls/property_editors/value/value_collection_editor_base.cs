////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for collection_editor.xaml
	/// </summary>
	public class value_collection_editor_base : value_editor_base
	{
		internal virtual void	remove_sub_property(property sub_property)
		{
			throw new NotImplementedException("methods must be implemented!");
		}
		internal virtual void move_up_sub_property(property sub_property)
		{
			
		}
		internal virtual void move_down_sub_property(property sub_property)
		{
			
		}
	}
}
