////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Reflection;
using System.Windows.Markup;

namespace xray.editor.wpf_controls.markup_extensions
{
	public class thisExtension: MarkupExtension
	{
		internal static void initialize( )
		{
			serv_type		= Type.GetType( "System.Windows.Markup.ProvideValueServiceProvider, PresentationFramework, Version=3.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35" );
			context_field	= serv_type.GetField( "_context", BindingFlags.Instance|BindingFlags.NonPublic|BindingFlags.Public );
			root_field		= typeof( ParserContext ).GetField( "_rootElement", BindingFlags.Instance|BindingFlags.NonPublic|BindingFlags.Public );
		}

		static Type			serv_type;
		static FieldInfo	context_field;
		static FieldInfo	root_field;

		public override object ProvideValue( IServiceProvider serviceProvider )
		{
			if( serviceProvider.GetType( ) != serv_type )
				return null;
			var context	= context_field.GetValue( serviceProvider );
			var root_element = root_field.GetValue( context );
			return root_element;
		}
	}
}
