////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.IO;
using System.Reflection;
using System.Windows;
using Microsoft.Win32;

namespace xray.editor.wpf_controls
{
	public static class settings
	{
		private static	Boolean			_curveEditorLockZoom				= false;
		private static	Double			_hierarchicalItemIndent				= 5;	

		public static	Boolean			curve_editor_lock_zoom				
		{
			get 
			{
				return _curveEditorLockZoom; 
			}
			set 
			{
				_curveEditorLockZoom = value; 
			}
		}
		public static	Double			hierarchical_item_indent			
		{
			get 
			{
				return _hierarchicalItemIndent; 
			}
			set 
			{
				_hierarchicalItemIndent = value; 
			}
		}

		public static	Thickness		hierarchical_item_indent_thickness	
		{
			get
			{
				return new Thickness( hierarchical_item_indent, 0, 0, 0 );
			}
		}

		private static	RegistryKey		get_sub_key	( this RegistryKey root, String name )	
		{
			return root.OpenSubKey( name, true ) ?? root.CreateSubKey( name );
		}

		public static	void			load		( RegistryKey key )						
		{
			//typeof( Path ).GetField("DirectorySeparatorChar", BindingFlags.NonPublic | BindingFlags.Static ).SetValue( null, '/' );
			//typeof( Path ).GetField("AltDirectorySeparatorChar", BindingFlags.NonPublic | BindingFlags.Static ).SetValue( null, '\\' );

			var controls_key			= key.get_sub_key			( "ControlsSettings" );
			var curve_editor_key		= controls_key.get_sub_key	( "CurveEditor" );

			curve_editor_lock_zoom		= Boolean.Parse( curve_editor_key.GetValue( "curve_editor_lock_zoom", false ).ToString( ) );
			hierarchical_item_indent	= Double.Parse( curve_editor_key.GetValue( "hierarchical_item_indent", 10 ).ToString( ) );

			curve_editor_key.Close		( );
			controls_key.Close			( );
		}
		public static	void			save		( RegistryKey key )						
		{
			var controls_key			= key.get_sub_key			( "ControlsSettings" );
			var curve_editor_key		= controls_key.get_sub_key	( "CurveEditor" );

			curve_editor_key.SetValue	( "curve_editor_lock_zoom", curve_editor_lock_zoom );
			curve_editor_key.SetValue	( "hierarchical_item_indent", hierarchical_item_indent );

			curve_editor_key.Close		( );
			controls_key.Close			( );
		}

	}
}
