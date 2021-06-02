////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.Reflection;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Forms.Integration;

namespace xray.editor.wpf_controls
{
	public static class utils
	{
		static	utils		( )			
		{
			_avalonAdapterType		= Type.GetType( "System.Windows.Forms.Integration.AvalonAdapter, WindowsFormsIntegration, Version=3.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35" );
			_hostControlFieldInfo	= _avalonAdapterType.GetField( "_hostControl", BindingFlags.NonPublic| BindingFlags.Instance );
		}

		private	static		Type		_avalonAdapterType;
		private	static		FieldInfo	_hostControlFieldInfo;

		public static		Point		get_screen_mouse_position	(  )									
		{
			utils_helpers.win_point pt;
			utils_helpers.GetCursorPos( out pt );

			return new Point( pt.x, pt.y );
		}
		public static		void		set_screen_mouse_position	( Point pt )							
		{
			utils_helpers.SetCursorPos( (Int32)pt.X, (Int32)pt.Y );
		}
		[DebuggerNonUserCode]
		public static		Object		cast_to						( this Object obj, Type type )			
		{
			if( obj.GetType( ) == type )
				return obj;

			if( obj is i_caster )
				return ( (i_caster)obj ).cast_to( type );
			
			return Convert.ChangeType( obj, type );
		}
		public static		void		swap_values<TType>			( ref TType first, ref TType second )	
		{
			var tmp		= first;
			first		= second;
			second		= tmp;
		}

		public static		Form		new_form_host				( String title, FrameworkElement child )				
		{
			var win_form		= new Form{ Text = title };
			var element_host	= new ElementHost { Dock = DockStyle.Fill, Child = child };
			win_form.Controls.Add( element_host );

			return win_form;
		}
		public static		Form		get_parent_form				( FrameworkElement element )			
		{
			while( !( element == null || element.GetType( ) == _avalonAdapterType ) )
				element = (FrameworkElement)element.Parent;
			
			if( element == null )
				return null;

			var host = (ElementHost)_hostControlFieldInfo.GetValue( element );

			return host.FindForm( );
		}
	}
}
