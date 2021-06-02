////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls.property_editors
{
	public class assignable_type_editor_picker: editor_picker
	{
		public assignable_type_editor_picker( Type edited_type, Type editor_type ):base( editor_type )
		{
			is_expandable		= false;
			this.edited_type		= edited_type;
		}

		public assignable_type_editor_picker( Type edited_type, Type editor_type , Boolean is_expandable ): base( editor_type )
		{
			this.is_expandable		= is_expandable;
			this.edited_type		= edited_type;
		}

		public		Boolean			is_expandable	{ get; set; }
		public		Type			edited_type		{ get; set; }

		protected override bool can_edit_internal( property property )
		{
			if ( property.type == typeof(Object) )
			{
				var value = property.value;
				if( value != null &&  edited_type.IsAssignableFrom( value.GetType( ) ) )
				{
					property.is_expandable_item = is_expandable;
					return true;
				}
			}

			if ( edited_type.IsAssignableFrom( property.type ) )
			{
				property.is_expandable_item = is_expandable;
				return true;
			}
			return false;
		}
	}
}
