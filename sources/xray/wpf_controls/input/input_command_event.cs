////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public class InputCommandEventArgs: RoutedEventArgs
	{
		public InputCommandEventArgs ( String command_string, RoutedEvent routed_event ): base( routed_event )
		{
			this.command_string = command_string;
		}

		public		String		command_string;
	}

	public delegate void InputCommandEventHandler( Object sender, InputCommandEventArgs e );
}
