////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;

namespace xray.editor.wpf_controls
{
	public static class input
	{
		public static readonly		RoutedEvent			PreviewInputCommandEvent = EventManager.RegisterRoutedEvent( "PreviewInputCommand", RoutingStrategy.Tunnel, typeof(InputCommandEventHandler), typeof(input) );
		public static readonly		RoutedEvent			InputCommandEvent = EventManager.RegisterRoutedEvent( "InputCommand", RoutingStrategy.Bubble, typeof(InputCommandEventHandler), typeof(input) );

		public static				void				AddPreviewInputCommandHandler		( DependencyObject d, InputCommandEventHandler handler )	
		{
			var uie = d as UIElement;
			if (uie != null)
			{
				uie.AddHandler( PreviewInputCommandEvent, handler );
			}
		}
		public static				void				RemovePreviewInputCommandHandler	( DependencyObject d, InputCommandEventHandler handler )	
		{
			var uie = d as UIElement;
			if (uie != null)
			{
				uie.RemoveHandler( PreviewInputCommandEvent, handler );
			}
		}
		public static				void				AddInputCommandHandler				( DependencyObject d, InputCommandEventHandler handler )	
		{
			var uie = d as UIElement;
			if (uie != null)
			{
				uie.AddHandler( InputCommandEvent, handler );
			}
		}
		public static				void				RemoveInputCommandHandler			( DependencyObject d, InputCommandEventHandler handler )	
		{
			var uie = d as UIElement;
			if (uie != null)
			{
				uie.RemoveHandler( InputCommandEvent, handler );
			}
		}

		public static				Boolean				RaiseInputCommandEvent				( String command, IInputElement element )					
		{
			var args = new InputCommandEventArgs( command, PreviewInputCommandEvent );
			element.RaiseEvent( args );

			if( !args.Handled )
			{
				args = new InputCommandEventArgs( command, InputCommandEvent );
				element.RaiseEvent( args );
			}

			return args.Handled;
		}
	}
}
