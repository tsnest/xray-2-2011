////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.control_containers
{
	public class dock_control_container: control_container
	{
		public Boolean is_dock_left;

		public dock_control_container( property_descriptor descriptor  ): base( descriptor )
		{
			
		}
		protected internal override Panel generate_panel()
		{
			var panel		= new DockPanel { LastChildFill = false };

			foreach( var control in controls )
			{
				var wpf_control		= control.Value.generate_control( );
				DockPanel.SetDock	( wpf_control, (is_dock_left) ? Dock.Left : Dock.Right );
				panel.Children.Add	( wpf_control );
			}

			return panel;
		}
	}
}
