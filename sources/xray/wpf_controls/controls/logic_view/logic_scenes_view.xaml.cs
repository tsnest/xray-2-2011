using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;

namespace xray.editor.wpf_controls.logic_view
{
	/// <summary>
	/// Interaction logic for logic_scenes_view.xaml
	/// </summary>
	public partial class logic_scenes_view
	{
		public logic_scenes_view( )
		{
			InitializeComponent( );

			logic_view_height = Height;
		}
		
		private	const		Double	min_view_size = 100;

		public Double logic_view_height 
		{
			get
			{
				return ActualHeight;
			}
			set
			{
				var static_offset = bottom_grip.ActualHeight + BorderThickness.Top + BorderThickness.Bottom;
				Height = value;
				logic_hypergraph.bottom_limit = value - static_offset;
			}
		} 

		private void on_bottom_drag(object sender, DragDeltaEventArgs e)
    	{
			var new_height = Height + e.VerticalChange;
			var static_offset = bottom_grip.ActualHeight + BorderThickness.Top + BorderThickness.Bottom;
			
			if ( new_height < logic_hypergraph.field.ActualHeight + static_offset )
			{
				new_height = logic_hypergraph.field.ActualHeight + static_offset;
			}
			
			if ( new_height < min_view_size )
			{
				new_height = min_view_size;
			}

    		logic_view_height = new_height;
    	}
}
}
