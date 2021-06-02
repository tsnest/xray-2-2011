using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows.Forms.Integration;

namespace xray.editor.wpf_controls
{
	public partial class time_layout_host: ElementHost
	{
		public time_layout_host()
		{
			in_constructor();
		}
		
		public		time_layout		m_time_layout;
		public new	String			Child;
		
		private void	in_constructor()
		{
			if (!IsDesignMode())
			{
				m_time_layout = new time_layout();
				base.Child = m_time_layout;
			}
		}

		private bool	IsDesignMode()
		{
			if (Site != null)
				return Site.DesignMode;

			StackTrace stackTrace = new StackTrace();
			int frameCount = stackTrace.FrameCount - 1;

			for (int frame = 0; frame < frameCount; frame++)
			{
				Type type = stackTrace.GetFrame(frame).GetMethod().DeclaringType;
				if (typeof(IDesignerHost).IsAssignableFrom(type))
					return true;
			}
			return false;
		}

		public void invalidate_child()
		{
			base.Child.InvalidateVisual();
		}

		public void set_new_time_layout_params(String parent_node_name, Action<float> set_length_time, Func<float> get_length_time, List<time_layout_item> items_list){
			m_time_layout.set_new_time_layout_params(parent_node_name, set_length_time,get_length_time,items_list);
		}
		
    }
}
