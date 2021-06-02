////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls.Primitives;

namespace xray.editor.wpf_controls.control_containers
{
	public class toggle_button: control
	{
		public toggle_button( String name ): base( name )
		{	
		}
		public				Object			content				
		{
			get;set;
		}
		public				Boolean?		state;

		public event		Action<toggle_button>	toggle;

		internal			void			on_toggle			( Object sender, RoutedEventArgs e )
		{
			state = ((ToggleButton)sender).IsChecked;

			if( toggle != null )
				toggle( this );
		}

		public override		FrameworkElement		generate_control	( )
		{
			wpf_control	= new ToggleButton{ Content = content };

			if( !Double.IsNaN( width ) )
				wpf_control.Width	= width;

			if( !Double.IsNaN( height ) )
				wpf_control.Height	= height;

			merge_to	( wpf_control );

			return wpf_control;
		}

		public override void merge_to( FrameworkElement ui_element )
		{
			wpf_control = ui_element;
			((ToggleButton)ui_element).Checked			+= on_toggle;
			((ToggleButton)ui_element).Unchecked		+= on_toggle;
			((ToggleButton)ui_element).Indeterminate	+= on_toggle;
		}
	}
}
