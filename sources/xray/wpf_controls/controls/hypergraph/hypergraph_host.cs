////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Windows;
using System.Windows.Forms.Integration;

namespace xray.editor.wpf_controls.hypergraph
{
	public class hypergraph_host : ElementHost
	{

		#region | Initialize |


		public hypergraph_host( )	
		{
			in_constructor();
		}


		#endregion

		#region |   Fields   |


		private hypergraph_control m_hypergraph;
		public new String Child;


		#endregion

		#region | Properties |


		public hypergraph_control		hypergraph	
		{
			get { return m_hypergraph; }
		}
		

		#endregion

		#region |  Methods   |


		private		void			in_constructor					( )													
		{
			if ( !is_design_mode( ) )
			{
				m_hypergraph	= new hypergraph_control( );
				base.Child		= m_hypergraph;
			}
		}
		private		bool			is_design_mode					( )													
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
