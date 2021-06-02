////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Input;

namespace xray.editor.wpf_controls.curve_editor.tools
{
	internal abstract class tool_base
	{
		public tool_base( curve_editor_panel parent_panel )
		{
			m_parent_panel = parent_panel;
		}

		protected			curve_editor_panel		m_parent_panel;
		protected			Boolean					m_is_in_action;

		public abstract		Boolean		mouse_down	( MouseButtonEventArgs e );
		public abstract		Boolean		mouse_move	( MouseEventArgs e );
		public abstract		Boolean		mouse_up	( MouseButtonEventArgs e );
	}
}
