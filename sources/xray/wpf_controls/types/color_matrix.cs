////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class color_matrix: sizeable_matrix<color_matrix_element>
	{
		public			color_matrix	( Size logical_size)
		{
			this.logical_size = logical_size;
		}
		public			Size			logical_size;
		public event	EventHandler	edit_complete;
		
		public void		invoke_edit_complete			( )
		{
			if( edit_complete != null )
				edit_complete( this, EventArgs.Empty );
		}
	}
}
