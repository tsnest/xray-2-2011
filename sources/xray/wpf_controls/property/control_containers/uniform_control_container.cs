////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Windows.Controls.Primitives;

namespace xray.editor.wpf_controls.control_containers
{
	public class uniform_control_container: control_container
	{
		public uniform_control_container ( property_descriptor descriptor  ): base( descriptor )
		{
			
		}

		protected internal override System.Windows.Controls.Panel generate_panel()
		{
			var grid		= new UniformGrid { Columns = controls.Count, Rows = 1 };

			foreach( var control in controls )
				grid.Children.Add( control.Value.generate_control( ) );

			return grid;
		}
	}
}
