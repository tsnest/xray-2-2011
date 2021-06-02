////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Markup;

namespace xray.editor.wpf_controls.markup_extensions
{
	public class template_from_typeExtension: MarkupExtension
	{
		public		template_from_typeExtension	( )				
		{
			
		}
		public		template_from_typeExtension	( Type type )	
		{
			this.type = type;
		}

		public		Type		type		
		{
			get;set;
		}
		public		Int32		fff
		{
			get;set;
		}

		public override		Object		ProvideValue	( IServiceProvider service_provider )	
		{
			return new DataTemplate{ VisualTree = new FrameworkElementFactory( type ) };
		}

	}
}
