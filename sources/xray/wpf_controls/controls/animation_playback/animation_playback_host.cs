////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Windows.Forms.Integration;
using System.Windows.Markup;

namespace xray.editor.wpf_controls.animation_playback
{
	public class animation_playback_host: ElementHost
	{

		#region | Initialize |


		public	animation_playback_host		()		
		{
			in_constructor();
		}


		#endregion

		#region |   Fields   |


		private animation_playback_panel	m_panel;
		public new String Child;


		#endregion

		#region | properties |


		public	animation_playback_panel		panel				
		{
			get { return m_panel; }
		}
		public	IEnumerable<animation_item>		animation_items		
		{
			get
			{
				return m_panel.animation_items;
			}
			set
			{
				m_panel.animation_items = value;
			}
		}


		#endregion

		#region |  Methods   |
		

		private void	in_constructor	()	
		{
			if (!is_design_mode())
			{
				m_panel = new animation_playback_panel();
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
