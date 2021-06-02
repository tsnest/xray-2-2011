////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Runtime.InteropServices;

namespace xray.editor.wpf_controls
{
	internal class utils_helpers
	{
		[DllImport("user32.dll")]
		internal static extern		Boolean		GetCursorPos	( out win_point pt );
		[DllImport("user32.dll")]
		internal static extern		Boolean		SetCursorPos	( Int32 x, Int32 y );

		[StructLayout(LayoutKind.Sequential)]
		internal struct win_point
		{
			public Int32 x;
			public Int32 y;

			public override string ToString()	
			{
				return String.Format( "{0:N}; {1:N}", x, y );
			}
		}
	}
}
