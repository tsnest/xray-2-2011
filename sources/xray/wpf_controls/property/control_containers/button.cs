////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.control_containers
{
	public class button: control
	{
		public button( String name ): base( name )
		{	
		}

		public				Object			content				
		{
			get;set;
		}
		public				Boolean			is_enabled			
		{
			get
			{
				return wpf_control.IsEnabled;
			}
			set
			{
				wpf_control.IsEnabled = value;
			}
		}

		public event		Action<button>	click;

		public override		FrameworkElement		generate_control	( )									
		{
			wpf_control	= new Button{ Content = content };

			if( !Double.IsNaN( width ) )
				wpf_control.Width	= width;

			if( !Double.IsNaN( height ) )
				wpf_control.Height	= height;

			merge_to	( wpf_control );

			return wpf_control;
		}
		public override		void					merge_to			( FrameworkElement ui_element )		
		{
			wpf_control = ui_element;
			((Button)ui_element).Click += on_click;
		}

		internal			void					on_click			( Object sender, RoutedEventArgs e )
		{
			if( click != null )
				click( this );
		}
	}
}
