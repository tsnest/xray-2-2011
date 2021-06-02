////////////////////////////////////////////////////////////////////////////
//	Created		: 27.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public static class dot_net_helpers
	{
		public static Boolean		is_type<TType, TObj>	( TObj obj )						
		{
			return obj is TType;
		}
		public static TType			as_type<TType, TObj>	( TObj obj ) where TType: class	
		{
			return obj as TType;
		}
	}
}
