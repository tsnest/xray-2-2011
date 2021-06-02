////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors
{
	public class editor_picker
	{
		public		editor_picker	( Type editor_type )		
		{
			this.editor_type		= editor_type;
		}

		public				Type			editor_type					
		{
			get;private set;
		}

		protected virtual	Boolean			can_edit_internal			( property property )	
		{
			return false;
		}
		public				Boolean			can_edit					( property property )	
		{
			return can_edit_internal( property );
		}
	}
}