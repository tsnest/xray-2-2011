using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms.Integration;
using xray.editor.wpf_controls.property_editors;

namespace xray.editor.wpf_controls.data_sources
{
	public class data_sources_panel_host : ElementHost
	{
			public data_sources_panel_host()
		{
			in_constructor();
		}
		
		public		data_sources_panel		m_data_sources_panel;
		public new	String					Child;
	
		private void	in_constructor()
		{
			if (!IsDesignMode())
			{
				m_data_sources_panel = new data_sources_panel();
				m_data_sources_panel.Margin = new System.Windows.Thickness(30,30,30,30);
				base.Child = m_data_sources_panel;
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

		public void add_data_source( Object  data_source ){
			if (m_data_sources_panel.data_sources.Items.Contains(data_source))
				return;

			m_data_sources_panel.data_sources.Items.Add( data_source );
		}
		
		public void remove_data_source( Object  data_source ){
			m_data_sources_panel.data_sources.Items.Remove( data_source );
		}

		public void clear_data_sources( ){
			m_data_sources_panel.data_sources.Items.Clear( );
		}


	}
}
