////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;

namespace xray.editor.wpf_controls.helpers
{
	internal static class debug
	{
		[Conditional("DEBUG")]
		[DebuggerNonUserCode]
		public static			void			assert					( Boolean value )					
		{
			if( value )
				return;

			if( !Debugger.IsAttached )
				throw new assertion_failed_exception( );

			Debugger.Log( 0, "Managed Assertion", "Assertion failed!" );
			Debugger.Break( );
		}

		[Conditional("DEBUG")]
		[DebuggerNonUserCode]
		public static			void			assert					( Boolean value, String message )	
		{
			if( value )
				return;

			if( !Debugger.IsAttached )
				throw new assertion_failed_exception( message );

			Debugger.Log( 0, "Managed Assertion", message );
			Debugger.Break( );
		}
	}

	internal class assertion_failed_exception: Exception
	{
		public	assertion_failed_exception	( ) : base( "Assertion failed!" )		
		{
		}
		public	assertion_failed_exception	( String message ): base( message )		
		{
		}
	}
}
