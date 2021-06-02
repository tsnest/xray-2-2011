////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Windows.Controls;

namespace xray.editor.wpf_controls.control_containers
{
	public class stack_control_container: control_container
	{
		public stack_control_container( property_descriptor descriptor  ): base( descriptor )
		{
			
		}

		protected internal override Panel generate_panel()
		{
			var panel		= new StackPanel { Orientation = Orientation.Horizontal };

			foreach( var control in controls )
				panel.Children.Add( control.Value.generate_control( ) );

			return panel;
		}
	}
}
