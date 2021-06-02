////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Windows.Forms.Integration;
using System.Windows.Markup;

namespace xray.editor.wpf_controls.animation_lexeme_panel
{
	public class animation_lexeme_panel_host: ElementHost
	{

		#region | Initialize |


		public	animation_lexeme_panel_host		()		
		{
			in_constructor();
		}


		#endregion

		#region |   Fields   |


		private animation_lexeme_panel	m_panel;
		public new String Child;


		#endregion

		#region | properties |


		public	animation_lexeme_panel		panel				
		{
			get { return m_panel; }
		}


		#endregion

		#region |  Methods   |
		

		private void	in_constructor	()	
		{
			if (!is_design_mode())
			{
				m_panel = new animation_lexeme_panel();
				base.Child = m_panel;
			}
		}
		private bool	is_design_mode	()	
		{
			if (Site != null)
				return Site.DesignMode;

			var stack_trace = new StackTrace();
			var frame_count = stack_trace.FrameCount - 1;

			for (int frame = 0; frame < frame_count; frame++)
			{
				Type type = stack_trace.GetFrame(frame).GetMethod().DeclaringType;
				if (typeof(IDesignerHost).IsAssignableFrom(type))
					return true;
			}
			return false;
		}


		#endregion

	}
}
