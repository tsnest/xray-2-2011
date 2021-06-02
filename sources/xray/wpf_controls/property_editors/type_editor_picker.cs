////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors
{
	public class type_editor_picker: editor_picker
	{
		public type_editor_picker( Type edited_type, Type editor_type ):base( editor_type )
		{
			this.edited_type		= edited_type;
		}

		public Type			edited_type { get; set; }

		protected override bool can_edit_internal(property property)
		{
			if ( property.type == typeof(Object) )
			{
				var value = property.value;
				if( value != null && value.GetType( ) == edited_type )
					return true;
			}

			if ( property.type == edited_type)
				return true;

			return false;
		}
	}
}
